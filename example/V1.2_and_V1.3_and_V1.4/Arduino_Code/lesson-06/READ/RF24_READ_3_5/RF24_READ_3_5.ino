#include <SPI.h>
#include <Wire.h>
#include <nRF24L01.h>
#include <RF24.h>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <driver/i2c.h>

/*---------------------------------------------------------------
 * LCD bus and panel configuration
 * The lesson keeps the display setup local to this file so the radio
 * receive logic can stay focused on the wireless data path.
 *--------------------------------------------------------------*/
class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9488     _panel_instance;
    lgfx::Bus_SPI       _bus_instance;   // SPI bus instance
    // lgfx::Touch_FT5x06  _touch_instance;
    // lgfx::Touch_GT911  _touch_instance;

  public:
    LGFX(void) {
      {
        auto cfg = _bus_instance.config();

        // SPI bus configuration
        cfg.spi_host = SPI2_HOST;  // Select SPI to use for ESP32-S2, C3: SPI2_HOST or SPI3_HOST / ESP32: VSPI_HOST or HSPI_HOST
        // As the ESP-IDF version upgrades, the descriptions VSPI_HOST and HSPI_HOST have been deprecated. If an error occurs, use SPI2_HOST or SPI3_HOST instead.
        cfg.spi_mode = 0;                    // Set SPI communication mode (0 ~ 3)
        cfg.freq_write = 40000000;            // SPI clock during transmission (max 80MHz, rounded down to value obtained by dividing 80MHz by an integer)
        cfg.freq_read = 16000000;             // SPI clock during reception
        cfg.spi_3wire = false;                // Set to true if receiving via MOSI pin
        cfg.use_lock = true;                  // Set to true if using transaction locks
        cfg.dma_channel = SPI_DMA_CH_AUTO;    // Set DMA channel to use (0=no DMA / 1=1ch / 2=2ch / SPI_DMA_CH_AUTO=automatic setting)
        // With ESP-IDF version upgrades, SPI_DMA_CH_AUTO is now recommended for the DMA channel.
        cfg.pin_sclk = 42;                    // Set SPI SCLK pin number
        cfg.pin_mosi = 39;                    // Set SPI MOSI pin number
        cfg.pin_miso = -1;                    // Set SPI MISO pin number (-1 = disable)
        cfg.pin_dc = 41;                      // Set SPI DC pin number (-1 = disable)

        _bus_instance.config(cfg);             // Apply settings to the bus.
        _panel_instance.setBus(&_bus_instance);  // Set bus on the panel.
      }

      { // Configure display panel settings.
        auto cfg = _panel_instance.config();  // Get display panel configuration structure.

        cfg.pin_cs = 40;    // CS pin number. (-1 = disable)
        cfg.pin_rst = 2;   // RST pin number. (-1 = disable)
        cfg.pin_busy = -1;  // BUSY pin number. (-1 = disable)

        // Default values are set for each panel, along with the BUSY pin number (-1 = disable). If you are unsure about an item, you can comment it out and try.

        cfg.memory_width = 320;    // Maximum width supported by the driver IC
        cfg.memory_height = 480;   // Maximum height supported by the driver IC
        cfg.panel_width = 320;     // Actual displayable width
        cfg.panel_height = 480;    // Actual displayable height
        cfg.offset_x = 0;          // Panel X-direction offset
        cfg.offset_y = 0;         // Panel Y-direction offset
        cfg.offset_rotation = 3;   // Offset value in rotation direction 0~7 (4~7 are inverted)
        cfg.dummy_read_pixel = 8;  // Dummy bits read before reading pixels
        cfg.dummy_read_bits = 1;   // Dummy bits read before reading non-pixel data
        cfg.readable = false;      // Set to true if data can be read
        cfg.invert = true;         // Set to true if panel colors are inverted
        cfg.rgb_order = false;      // Set to true if panel red and blue are swapped
        cfg.dlen_16bit = false;    // Set to true for panels that send data length in 16-bit units
        cfg.bus_shared = true;    // Set to true if the bus is shared with an SD card (use bus control when using drawJpgFile etc.)

        _panel_instance.config(cfg);
      }

      
      setPanel(&_panel_instance);
    }
};

// Global display object used for status text.
LGFX gfx;

// nRF24L01 CE pin.
#define CE_PIN 1
// nRF24L01 CSN pin.
#define CSN_PIN 2

// Radio object that receives text payloads from the transmitter board.
RF24 radio(CE_PIN, CSN_PIN);

// Custom HSPI instance used by the RF24 module.
SPIClass* hspi = nullptr;

// HSPI pin mapping for the wireless module.
#define HSPI_MISO  9
#define HSPI_MOSI  3
#define HSPI_SCLK  10
#define HSPI_SS    46

/**
 * @brief Clear the LCD and print a received message.
 *
 * The lesson uses the displayed counter text to make every received
 * packet visibly distinct.
 *
 * @param lcd_w LCD width kept for call compatibility.
 * @param lcd_h LCD height kept for call compatibility.
 * @param x X coordinate for the text cursor.
 * @param y Y coordinate for the text cursor.
 * @param text Text content to render.
 * @return None.
 * @note Called after each valid RF24 packet is received.
 */
void show_test(int lcd_w, int lcd_h, int x, int y, const char * text)
{
  gfx.fillScreen(TFT_BLACK);
  gfx.setTextSize(3);
  gfx.setTextColor(TFT_RED);
  gfx.setCursor(x, y);
  gfx.print(text);
}

// RF24 payload identifier used by both boards.
const byte address[6] = "00001";

/**
 * @brief Prepare the LCD, wireless module, and receive pipeline.
 *
 * The board first turns on the backlight, then configures the nRF24L01
 * receiver on a custom HSPI bus. If the radio does not respond, the
 * lesson stops so students can check wiring before continuing.
 *
 * @param None.
 * @return None.
 * @note Called once by the Arduino runtime after reset.
 */
void setup() {
  Serial.begin(115200);

  Wire.begin(15, 16);
  delay(50);

  pinMode(38, OUTPUT);  //  Backlight pin
  digitalWrite(38, HIGH);

  /*Switch GPIO45 to low level to enable wireless module*/
  pinMode(45, OUTPUT);
  digitalWrite(45, LOW);// Switch between microphone and wireless module

  /*---------------------------------------------------------------
   * Initialize the LCD before waiting for wireless traffic.
   * This ensures the board can immediately show status text when a
   * packet arrives.
   *--------------------------------------------------------------*/
  gfx.init();
  gfx.initDMA();
  gfx.startWrite();
  gfx.fillScreen(TFT_BLACK);

  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }

  hspi = new SPIClass(HSPI); // by default VSPI is used
  hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);

  if (!radio.begin(hspi)) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  else
  {
    Serial.println(F("radio hardware is OK!!"));
  }
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(50);
  radio.startListening();
}

// Counts received packets so each screen update is easy to tell apart.
int i=0;

/**
 * @brief Poll the radio and update the screen when a packet arrives.
 *
 * Every valid payload is printed to the serial monitor and then shown
 * on the LCD together with a counter. This gives students two ways to
 * verify the same wireless transfer.
 *
 * @param None.
 * @return None.
 * @note Called repeatedly by the Arduino runtime.
 */
void loop() {                                          
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
    String str = text;
    str += String(i);
    show_test(480, 320, 50, 100, str.c_str());
    i++;
  }
}
