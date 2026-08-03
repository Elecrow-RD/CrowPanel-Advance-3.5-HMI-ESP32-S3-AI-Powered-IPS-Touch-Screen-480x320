#include <SPI.h>                 // Include SPI library for SPI communication
#include <Wire.h>                // Include Wire library for I2C communication
#include <nRF24L01.h>             // Include nRF24L01 register definitions
#include <RF24.h>                // Include RF24 library for nRF24L01 control

#define LGFX_USE_V1               // Use LovyanGFX version 1 API
#include <LovyanGFX.hpp>          // Main LovyanGFX header
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp> // RGB panel support for ESP32-S3
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>   // RGB bus support for ESP32-S3
#include <driver/i2c.h>           // ESP-IDF I2C driver definitions

/*---------------------------------------------------------------
 * LCD bus and panel configuration
 * The display setup lives here so the transmit path stays focused on
 * the wireless data flow.
 *--------------------------------------------------------------*/
// Custom display driver class inheriting from LovyanGFX base device
class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9488 _panel_instance; // ILI9488 LCD panel instance
    lgfx::Bus_SPI      _bus_instance;    // SPI bus instance for LCD communication

  public:
    // Constructor: configure SPI bus and LCD panel
    LGFX(void) {
      {
        auto cfg = _bus_instance.config();  // Get SPI bus configuration structure

        // SPI bus configuration
        cfg.spi_host = SPI2_HOST;            // Select SPI host (SPI2_HOST or SPI3_HOST for ESP32-S3)
        // VSPI_HOST / HSPI_HOST are deprecated in newer ESP-IDF versions
        cfg.spi_mode = 0;                    // SPI mode (0-3)
        cfg.freq_write = 40000000;           // SPI write clock frequency (40 MHz)
        cfg.freq_read  = 16000000;           // SPI read clock frequency (16 MHz)
        cfg.spi_3wire  = false;              // Set true if using MOSI for read (3-wire SPI)
        cfg.use_lock   = true;               // Enable SPI transaction lock
        cfg.dma_channel = SPI_DMA_CH_AUTO;   // Automatically select DMA channel
        // SPI_DMA_CH_AUTO is recommended for newer ESP-IDF versions

        cfg.pin_sclk = 42;                   // SPI SCLK pin
        cfg.pin_mosi = 39;                   // SPI MOSI pin
        cfg.pin_miso = -1;                   // SPI MISO pin (-1 = disabled)
        cfg.pin_dc   = 41;                   // Data/Command pin (-1 = disabled)

        _bus_instance.config(cfg);           // Apply SPI bus configuration
        _panel_instance.setBus(&_bus_instance); // Attach SPI bus to the panel
      }

      { 
        // Configure LCD panel parameters
        auto cfg = _panel_instance.config(); // Get panel configuration structure

        cfg.pin_cs   = 40;                   // Chip Select pin (-1 = disabled)
        cfg.pin_rst  = 2;                    // Reset pin (-1 = disabled)
        cfg.pin_busy = -1;                   // Busy pin (-1 = disabled)

        // Display resolution and panel settings
        cfg.memory_width  = 320;             // Maximum width supported by the driver IC
        cfg.memory_height = 480;             // Maximum height supported by the driver IC
        cfg.panel_width   = 320;             // Actual visible width
        cfg.panel_height  = 480;             // Actual visible height
        cfg.offset_x = 0;                    // Horizontal offset
        cfg.offset_y = 0;                    // Vertical offset
        cfg.offset_rotation = 3;             // Rotation offset (0-7, 4-7 are inverted)
        cfg.dummy_read_pixel = 8;             // Dummy bits before reading pixel data
        cfg.dummy_read_bits  = 1;             // Dummy bits before reading non-pixel data
        cfg.readable   = false;              // Set true if the panel supports read-back
        cfg.invert     = true;               // Invert display colors if required
        cfg.rgb_order  = false;              // Set true if red/blue color order is swapped
        cfg.dlen_16bit = false;              // True if panel uses 16-bit length units
        cfg.bus_shared = true;               // True if SPI bus is shared (e.g. with SD card)

        _panel_instance.config(cfg);          // Apply panel configuration
      }

      setPanel(&_panel_instance);             // Register the panel with LovyanGFX
    }
};

// Create global display object
LGFX gfx;

// nRF24L01 CE pin.
#define CE_PIN  1                             // nRF24L01 CE pin
// nRF24L01 CSN pin.
#define CSN_PIN 2                             // nRF24L01 CSN pin

// Radio object that sends text payloads to the receiver board.
RF24 radio(CE_PIN, CSN_PIN);

// Custom HSPI instance used by the wireless module.
SPIClass* hspi = nullptr;                     // Pointer for HSPI instance

// HSPI pin mapping for the wireless module.
#define HSPI_MISO  9                          // HSPI MISO pin
#define HSPI_MOSI  3                          // HSPI MOSI pin
#define HSPI_SCLK  10                         // HSPI SCLK pin
#define HSPI_SS    46                         // HSPI SS pin

/**
 * @brief Clear the LCD and print the outgoing message counter.
 *
 * Showing the same text that is transmitted makes it easier to match
 * the serial log, the LCD, and the receiver board.
 *
 * @param lcd_w LCD width kept for call compatibility.
 * @param lcd_h LCD height kept for call compatibility.
 * @param x X coordinate for the text cursor.
 * @param y Y coordinate for the text cursor.
 * @param text Text content to render.
 * @return None.
 * @note Called after each transmission cycle.
 */
void show_test(int lcd_w, int lcd_h, int x, int y, const char * text)
{
  gfx.fillScreen(TFT_BLACK);                  // Clear screen with black color
  gfx.setTextSize(3);                         // Set text size
  gfx.setTextColor(TFT_RED);                  // Set text color to red
  gfx.setCursor(x, y);                        // Set text cursor position
  gfx.print(text);                            // Print text on the display
}

// RF24 payload identifier used by both boards.
const byte address[6] = "00001";              // RF24 communication address

/**
 * @brief Prepare the transmitter board and start the radio link.
 *
 * The lesson enables the backlight, configures the wireless module on
 * a custom HSPI bus, and then switches the radio into transmit mode.
 *
 * @param None.
 * @return None.
 * @note Called once by the Arduino runtime after reset.
 */
void setup() {
  Serial.begin(115200);                       // Initialize serial communication

  Wire.begin(15, 16);                         // Initialize I2C with SDA=15, SCL=16
  delay(50);                                  // Short delay for stabilization

  pinMode(38, OUTPUT);                        // Configure backlight pin as output
  digitalWrite(38, HIGH);                     // Turn on LCD backlight

  /* Switch GPIO45 to low level to enable wireless module */
  pinMode(45, OUTPUT);                        // Configure GPIO45 as output
  digitalWrite(45, LOW);                      // Select wireless module (disable microphone)

  // Initialize LCD
  gfx.init();                                 // Initialize display
  gfx.initDMA();                              // Enable DMA for faster drawing
  gfx.startWrite();                           // Start SPI write transaction
  gfx.fillScreen(TFT_BLACK);                  // Clear screen

  while (!Serial) {
    // Wait for USB serial connection if required
  }

  hspi = new SPIClass(HSPI);                  // Create HSPI instance
  hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);

  if (!radio.begin(hspi)) {                  // Initialize RF24 using HSPI
    Serial.println(F("radio hardware is not responding!!")); // Error message
    while (1) {}                              // Halt execution
  }
  else
  {
    Serial.println(F("radio hardware is OK!!")); // RF24 initialization successful
  }

  radio.openWritingPipe(address);             // Set RF24 TX address
  radio.setPALevel(RF24_PA_MAX);               // Set power amplifier level to maximum
  radio.setDataRate(RF24_250KBPS);             // Set data rate (250Kbps)
  radio.setChannel(50);                       // Set RF channel
  radio.stopListening();                      // Set module to transmit mode
}

// Counts sent packets so the screen and serial output change visibly.
int i = 0;

/**
 * @brief Send a packet every second and mirror the status on the LCD.
 *
 * The board repeatedly transmits a short text payload, which lets
 * students see the transmitter side, receiver side, and serial log all
 * change together.
 *
 * @param None.
 * @return None.
 * @note Called repeatedly by the Arduino runtime.
 */
void loop() {
  Serial.println(F("SENDING..."));
  String str = "SENDING...";
  str += String(i);
  show_test(480, 320, 150, 150, str.c_str());
  i++;

  const char text[] = "Hello World I2:";
  radio.write(&text, sizeof(text));
  delay(1000);
}
