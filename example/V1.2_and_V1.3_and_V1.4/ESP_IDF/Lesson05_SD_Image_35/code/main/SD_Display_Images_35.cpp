#include <Arduino.h>
#include "LovyanGFX_Driver.h"
#include <Wire.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>

#define LCD_H_RES 480
#define LCD_V_RES 320

/*---------------------------------------------------------------
 * SD card SPI pin map
 * The BMP files are loaded from the TF card over this SPI bus.
 *--------------------------------------------------------------*/
#define SD_MOSI 6
#define SD_MISO 4
#define SD_SCK  5
#define SD_CS   7 //The chip selector pin is not connected to IO

// LCD backlight control pin.
#define LCD_BL_PIN 38

/*---------------------------------------------------------------
 * BMP image files expected on the SD card
 * The files must be copied to the root directory with these names.
 *--------------------------------------------------------------*/
#define IMAGE_1 "/1.bmp"
#define IMAGE_2 "/2.bmp"
#define IMAGE_3 "/3.bmp"
#define IMAGE_4 "/4.bmp"
#define IMAGE_5 "/5.bmp"

// Dedicated SPI object used for SD card access.
SPIClass SD_SPI = SPIClass(HSPI);

// LCD driver instance used by the image display helpers.
static LGFX lcd;

/**
 * @brief Clear the LCD and print a status message.
 *
 * @param lcd_w LCD width kept for call compatibility.
 * @param lcd_h LCD height kept for call compatibility.
 * @param x X coordinate for the text cursor.
 * @param y Y coordinate for the text cursor.
 * @param text Message to render.
 * @return None.
 * @note Called during setup after SD card initialization.
 */
void show_test(int lcd_w, int lcd_h, int x, int y, const char * text)
{
  lcd.setRotation(0);
  lcd.fillScreen(TFT_BLACK);
  lcd.setTextSize(3);
  lcd.setTextColor(TFT_RED);
  lcd.setCursor(x, y);
  lcd.print(text); 
}

/**
 * @brief Print files and subdirectories from an SD card directory.
 *
 * The listing confirms that the expected BMP files are present before
 * the slideshow attempts to open them.
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

          // Limit recursion so the lesson lists useful content without
          // walking an unexpectedly deep card.
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
 * @brief Mount the SD card and list its root directory.
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
    Serial.printf("SD Size： %lluMB \n", SD.cardSize() / (1024 * 1024));
  }

  listDir(SD, "/", 2);
  Serial.println("**** TF Card init finished ****.");
  return 0;
}

/**
 * @brief Read a 24-bit BMP file and draw it row by row.
 *
 * The BMP header is skipped, then one RGB888 row is loaded into RAM
 * and pushed to the LCD. Row-by-row drawing keeps memory use small.
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

  for (int row = 0; row < Y; row++)

  {
    f.seek(54 + 3 * X * row);
    f.read(RGB, 3 * X);
    lcd.pushImage(0, row, X, 1, (lgfx::rgb888_t *)RGB);
  }

  f.close();
  return 0; 
}

/**
 * @brief Initialize serial output, LCD, backlight, and the SD card.
 *
 * The panel pointer and begin result are checked explicitly so a
 * display driver problem is separated from SD card or BMP file issues.
 *
 * @param None.
 * @return None.
 * @note Called once by the Arduino runtime through app_main().
 */
void setup()
{
  Serial.begin(115200);

  delay(300);
  pinMode(LCD_BL_PIN, OUTPUT);
  digitalWrite(LCD_BL_PIN, HIGH);

  lcd.configure();

  Serial.printf("Panel: %p\n", lcd.getPanel());
  if (lcd.getPanel() == nullptr)
  {
    Serial.println("Panel pointer is null after configure!");
    while (true) { delay(1000); }
  }

  if (!lcd.begin())
  {
    Serial.println("Display init failed!");
    while (true) { delay(1000); }
  }

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

  lcd.setRotation(2);
  lcd.fillScreen(TFT_BLACK);
  Serial.println( "----- Setup done -----" );

}

/**
 * @brief Display the five BMP files in a repeating slideshow.
 *
 * @param None.
 * @return None.
 * @note Called repeatedly by app_main().
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

extern "C" void app_main(void)
{
    initArduino();

    setup();
    while (true)
    {
        loop();
        delay(1);
    }
}
