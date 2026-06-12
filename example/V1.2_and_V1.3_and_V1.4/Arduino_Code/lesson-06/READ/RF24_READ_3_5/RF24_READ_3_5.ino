#include <SPI.h>
#include <Wire.h>
#include <nRF24L01.h>
#include <RF24.h>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <driver/i2c.h>

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

LGFX gfx;

#define CE_PIN 1
#define CSN_PIN 2

// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);

SPIClass* hspi = nullptr;

#define HSPI_MISO  9
#define HSPI_MOSI  3
#define HSPI_SCLK  10
#define HSPI_SS    46

/*
Function: Display text on the screen
    lcd_w: Product horizontal axis resolution
    lcd_h: Product vertical axis resolution
    x: Screen display starting horizontal axis
    y: Screen display starting vertical axis
    text: The text content displayed on the screen
*/
void show_test(int lcd_w, int lcd_h, int x, int y, const char * text)
{
  gfx.fillScreen(TFT_BLACK);
  gfx.setTextSize(3);
  gfx.setTextColor(TFT_RED);
  gfx.setCursor(x, y);
  gfx.print(text);
}

const byte address[6] = "00001";
void setup() {
  Serial.begin(115200);

  Wire.begin(15, 16);
  delay(50);

  pinMode(38, OUTPUT);  //  Backlight pin
  digitalWrite(38, HIGH);

  /*Switch GPIO45 to low level to enable wireless module*/
  pinMode(45, OUTPUT);
  digitalWrite(45, LOW);// Switch between microphone and wireless module

  // Init Display
  gfx.init();
  gfx.initDMA();
  gfx.startWrite();
  gfx.fillScreen(TFT_BLACK);

  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }

  hspi = new SPIClass(HSPI); // by default VSPI is used
  // to use the custom defined pins, uncomment the following
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
  radio.setDataRate(RF24_250KBPS);  //RF24_250KBPS  RF24_1MBPS  RF24_2MBPS
  radio.setChannel(50);
  radio.startListening();
}

int i=0;
void loop() {                                          
  //  Serial.println(F("READ !!"));
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));//  Read the content of the text sent over
    Serial.println(text);
    String str = text;
    str += String(i);
    show_test(480, 320, 50, 100, str.c_str());
    i++;
  }
}
