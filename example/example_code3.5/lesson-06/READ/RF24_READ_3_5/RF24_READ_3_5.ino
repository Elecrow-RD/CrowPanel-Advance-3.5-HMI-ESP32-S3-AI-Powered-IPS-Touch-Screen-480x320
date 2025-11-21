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
    lgfx::Bus_SPI           _bus_instance; 
    // lgfx::Touch_GT911    _touch_instance;

  public:
    LGFX(void) {
      {
        auto cfg = _bus_instance.config();

        // SPI bus configuration
        cfg.spi_host = SPI2_HOST;  // Select SPI host. For ESP32-S2/C3: SPI2_HOST or SPI3_HOST. For ESP32: VSPI_HOST or HSPI_HOST.
        // With ESP-IDF updates, VSPI_HOST and HSPI_HOST are deprecated. If errors appear, use SPI2_HOST or SPI3_HOST instead.
        cfg.spi_mode = 0;                     // Set SPI communication mode (0 ~ 3)
        cfg.freq_write = 40000000;            // SPI clock for write (max 80MHz, rounded from 80MHz divided by integer)
        cfg.freq_read = 16000000;             // SPI clock for read
        cfg.spi_3wire = false;                // Set true if reading through MOSI pin
        cfg.use_lock = true;                  // Set true to use transaction lock
        cfg.dma_channel = SPI_DMA_CH_AUTO;    // DMA channel (0 = disabled / 1 = ch1 / 2 = ch2 / SPI_DMA_CH_AUTO = auto-select)
        // In newer ESP-IDF versions, SPI_DMA_CH_AUTO is recommended for automatic DMA assignment
        cfg.pin_sclk = 42;                    // SPI SCLK pin
        cfg.pin_mosi = 39;                    // SPI MOSI pin
        cfg.pin_miso = -1;                    // SPI MISO pin (-1 = disable)
        cfg.pin_dc = 41;                      // SPI DC pin (-1 = disable)

        _bus_instance.config(cfg);               // Apply bus configuration
        _panel_instance.setBus(&_bus_instance);  // Attach the SPI bus to the panel
      }

      { // Display panel control settings
        auto cfg = _panel_instance.config();  // Get panel configuration structure

        cfg.pin_cs = 40;     // CS pin number (-1 = disable)
        cfg.pin_rst = 2;     // RST pin number (-1 = disable)
        cfg.pin_busy = -1;   // BUSY pin number (-1 = disable)

        // Default values are set for each panel. If uncertain, you may comment out some items for testing.

        cfg.memory_width = 320;     // Max width supported by the driver IC
        cfg.memory_height = 480;    // Max height supported by the driver IC
        cfg.panel_width = 320;      // Actual visible width
        cfg.panel_height = 480;     // Actual visible height
        cfg.offset_x = 0;           // X-axis offset of the panel
        cfg.offset_y = 0;           // Y-axis offset of the panel
        cfg.offset_rotation = 3;    // Offset of rotation direction (0~7, 4~7 are upside-down)
        cfg.dummy_read_pixel = 8;   // Dummy bits before reading pixels
        cfg.dummy_read_bits = 1;    // Dummy bits before reading non-pixel data
        cfg.readable = false;       // Set true if panel supports reading data
        cfg.invert = true;          // Set true if panel brightness is reversed
        cfg.rgb_order = false;      // Set true if red/blue order needs to be swapped
        cfg.dlen_16bit = false;     // Set true if panel sends data in 16-bit length units
        cfg.bus_shared = true;      // Set true if bus is shared with SD card (drawJpgFile etc. will manage bus)

        _panel_instance.config(cfg);
      }

      // Touch panel configuration (commented out)
      // {
      //   auto cfg = _touch_instance.config();
      //   cfg.x_min = 0;
      //   cfg.x_max = 319;
      //   cfg.y_min = 0;
      //   cfg.y_max = 479;
      //   cfg.pin_int = 47;
      //   cfg.bus_shared = false;
      //   cfg.offset_rotation = 0;
      //   // I2C connection
      //   cfg.i2c_port = I2C_NUM_0;
      //   cfg.pin_sda = GPIO_NUM_15;
      //   cfg.pin_scl = GPIO_NUM_16;
      //   cfg.pin_rst = 48;
      //   cfg.freq = 400000;
      //   cfg.i2c_addr = 0x14;  // 0x5D , 0x14
      //   _touch_instance.config(cfg);
      //   _panel_instance.setTouch(&_touch_instance);
      // }

      setPanel(&_panel_instance);   // Attach the panel to LGFX device
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
Function function: Display text on the screen
    lcd_w: Product horizontal axis resolution
    lcd_h： Product vertical axis resolution
    x： Screen displays the starting horizontal axis
    y： Screen displays the starting vertical axis
    text： The text content displayed on the screen
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

 /*Switch GPO45 to low level to enable wireless module*/
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
