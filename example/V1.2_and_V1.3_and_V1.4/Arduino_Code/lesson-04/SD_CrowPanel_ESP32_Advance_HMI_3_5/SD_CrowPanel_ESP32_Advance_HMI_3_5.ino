#include "pins_config.h"
#include "LovyanGFX_Driver.h"
#include <Wire.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include <Arduino.h>

/*---------------------------------------------------------------
 * SD card SPI pin map
 * The TF card uses a separate SPI bus so image reads do not conflict
 * with the LCD configuration.
 *--------------------------------------------------------------*/
#define SD_MOSI 6
#define SD_MISO 4
#define SD_SCK  5
#define SD_CS   7 //The chip selector pin is not connected to IO

/*---------------------------------------------------------------
 * BMP image files expected on the SD card
 * These names must match the files copied to the card root directory.
 *--------------------------------------------------------------*/
#define IMAGE_1 "/1.bmp"
#define IMAGE_2 "/2.bmp"
#define IMAGE_3 "/3.bmp"
#define IMAGE_4 "/4.bmp"
#define IMAGE_5 "/5.bmp"

// Dedicated SPI object used for the SD card.
SPIClass SD_SPI = SPIClass(HSPI);

// Global display driver instance used by the lesson.
LGFX gfx;

/**
 * @brief Clear the screen and print a status message.
 *
 * The lesson uses this helper to show whether the SD card was mounted
 * successfully before the image slideshow starts.
 *
 * @param lcd_w LCD width kept for compatibility with the lesson call.
 * @param lcd_h LCD height kept for compatibility with the lesson call.
 * @param x Text start coordinate on the X axis.
 * @param y Text start coordinate on the Y axis.
 * @param text Message to display.
 * @return None.
 * @note Called during setup after SD card initialization.
 */
void show_test(int lcd_w, int lcd_h, int x, int y, const char * text)
{
  gfx.fillScreen(TFT_BLACK);
  gfx.setTextSize(3);
  gfx.setTextColor(TFT_RED);
  gfx.setCursor(x, y);
  gfx.print(text); 
}

/**
 * @brief Initialize the LCD and verify the SD card before playback.
 *
 * The display is prepared first so the board can show a clear success
 * or failure message for SD card mounting. After that, the slideshow
 * begins from a black screen.
 *
 * @param None.
 * @return None.
 * @note Called once by the Arduino runtime after reset.
 */
void setup()
{
  Serial.begin(115200);

  /*---------------------------------------------------------------
   * Initialize LCD output and make the screen visible.
   * DMA is enabled for faster image transfers, and the backlight is
   * turned on only after the panel has been cleared.
   *--------------------------------------------------------------*/
  gfx.init();
  gfx.initDMA();
  gfx.startWrite();
  gfx.fillScreen(TFT_BLACK);
  delay(500);

  /* Turn on backlight */
  pinMode(38, OUTPUT);
  digitalWrite(38, HIGH);

  if (SD_init() == 0)
  {
    Serial.println("TF card initialization succeeded");
    show_test(LCD_H_RES, LCD_V_RES, 125, 135, "SD_Card OK");
    delay(3000);
  } else {
    Serial.println("TF card initialization failed");
    show_test(LCD_H_RES, LCD_V_RES, 125, 135, "SD_Card failed");
    delay(3000);
  }
  gfx.setRotation(2);// 7
  gfx.fillScreen(TFT_BLACK);
  Serial.println( "----- Setup done -----" );
}

/**
 * @brief Display the five BMP images in a repeating slideshow.
 *
 * Each image remains on screen for five seconds. The serial messages
 * make it easy to match the visible picture with the file currently
 * being read from the SD card.
 *
 * @param None.
 * @return None.
 * @note Called repeatedly by the Arduino runtime.
 */
void loop()
{
  Serial.println("Refreshing image...1");
  displayImage(SD, IMAGE_1, 480, 320);
  delay(5000);

  Serial.println("Refreshing image...2");
  displayImage(SD, IMAGE_2, 480, 320);
  delay(5000);

  Serial.println("Refreshing image...3");
  displayImage(SD, IMAGE_3, 480, 320);
  delay(5000);

  Serial.println("Refreshing image...4");
  displayImage(SD, IMAGE_4, 480, 320);
  delay(5000);

  Serial.println("Refreshing image...5");
  displayImage(SD, IMAGE_5, 480, 320);
  delay(5000);
}

/**
 * @brief Mount the SD card and print its directory contents.
 *
 * A successful mount proves that the SPI wiring, chip select pin, and
 * file system are usable before the program tries to read BMP data.
 *
 * @param None.
 * @return 0 if the SD card is mounted successfully.
 * @return 1 if mounting fails.
 * @note Called once during setup.
 */
int SD_init()
{
  SD_SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
  if (!SD.begin(SD_CS, SD_SPI, 80000000))
  {
    Serial.println(F("ERROR: File system mount failed!"));
    SD_SPI.end();
    return 1;
  }
  else
  {
    Serial.println("Card Mount Successed");
    Serial.printf("SD Size: %llu MB\n", SD.cardSize() / (1024 * 1024));
  }
  listDir(SD, "/", 2);
  Serial.println("**** TF Card init finished ****.");
  return 0;
}

/**
 * @brief Print files and subdirectories from an SD card directory.
 *
 * The directory listing is a quick check that the expected BMP files
 * are actually present on the card before the display routine uses
 * their paths.
 *
 * @param fs File system object used for SD card access.
 * @param dirname Directory path to list.
 * @param levels Remaining recursive depth for subdirectories.
 * @return None.
 * @note Called by SD_init() after the card is mounted.
 */
void listDir(fs::FS & fs, const char *dirname, uint8_t levels) {
    Serial.printf("Listing directory: %s\n", dirname); 
    File root = fs.open(dirname);
    if (!root) { 
      Serial.println("Failed to open directory");
      return;
    }

    if (!root.isDirectory()) { 
      Serial.println("Not a directory"); 
      return; 
    }

    File file = root.openNextFile();
    while (file) { 
      if (file.isDirectory()) { 
          Serial.print("  DIR : "); 
          Serial.println(file.name());

          // Recurse only while depth remains, preventing an unlimited
          // directory walk on cards with many nested folders.
          if (levels) { 
              listDir(fs, file.name(), levels - 1);
          }
      } 
      else { 
          Serial.print("  FILE: "); 
          Serial.print(file.name());
          Serial.print("  SIZE: "); 
          Serial.println(file.size());
      }
      file = root.openNextFile();
    }
}


/**
 * @brief Read a 24-bit BMP file and draw it to the LCD.
 *
 * The BMP header is skipped, then each image row is read as RGB888
 * data and pushed to the display. If the file path is wrong or the SD
 * card is missing, the function prints an error and leaves the current
 * screen contents unchanged.
 *
 * @param fs File system object used for SD card access.
 * @param filename BMP file path on the SD card.
 * @param x Image width in pixels.
 * @param y Image height in pixels.
 * @return 0 after the function finishes.
 * @note Called by loop() for each slideshow image.
 */
int displayImage(fs::FS &fs, String filename, int x, int y)
{
  File f = fs.open(filename, "r");
  if (!f)
  {
    Serial.println("Failed to open file for reading");
    f.close();
    return 0;
  }

  f.seek(54);
  int X = x;
  int Y = y;
  uint8_t RGB[3 * X];

  // Draw one row at a time to keep RAM usage small while still showing
  // a full-screen bitmap.
  for (int row = 0; row < Y; row++)
  {
    f.seek(54 + 3 * X * row);
    f.read(RGB, 3 * X);
    gfx.pushImage(0, row, X, 1, (lgfx::rgb888_t *)RGB);
  }

  f.close();
  return 0; 
}
