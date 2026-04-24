#define LGFX_USE_V1
#include <LovyanGFX.hpp>
//#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
//#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <driver/i2c.h>

class LGFX : public lgfx::LGFX_Device
{
    // lgfx::Panel_ST7789     _panel_instance;
    // lgfx::Panel_ST7796     _panel_instance;
    // lgfx::Panel_ST7735     _panel_instance;
    lgfx::Panel_ILI9488     _panel_instance;
    // lgfx::Panel_ILI9341     _panel_instance;
    lgfx::Bus_SPI       _bus_instance;   // SPI bus instance
    // lgfx::Touch_FT5x06  _touch_instance;
    lgfx::Touch_GT911  _touch_instance;

  public:
    LGFX(void) {
      {
        auto cfg = _bus_instance.config();

        // SPI bus configuration
        cfg.spi_host = SPI2_HOST;  // Select SPI host: for ESP32-S2, C3 use SPI2_HOST or SPI3_HOST / for ESP32 use VSPI_HOST or HSPI_HOST
        // With newer ESP-IDF versions, VSPI_HOST and HSPI_HOST are deprecated. Use SPI2_HOST or SPI3_HOST if errors occur.
        cfg.spi_mode = 0;                     // Set SPI communication mode (0 ~ 3)
        cfg.freq_write = 40000000;            // SPI clock for writing (max 80MHz, rounded from 80MHz divided by integer)
        cfg.freq_read = 16000000;             // SPI clock for reading
        cfg.spi_3wire = false;                // Set true if receiving via MOSI pin
        cfg.use_lock = true;                  // Set true to use transaction lock
        cfg.dma_channel = SPI_DMA_CH_AUTO;    // Set DMA channel (0=not used / 1=ch1 / 2=ch2 / SPI_DMA_CH_AUTO=auto)
        // It is now recommended to use SPI_DMA_CH_AUTO in new ESP-IDF versions.
        cfg.pin_sclk = 42;                    // Set SPI SCLK pin number
        cfg.pin_mosi = 39;                    // Set SPI MOSI pin number
        cfg.pin_miso = -1;                    // Set SPI MISO pin number (-1 = disable)
        cfg.pin_dc = 41;                      // Set SPI DC pin number (-1 = disable)

        _bus_instance.config(cfg);               // Apply settings to bus
        _panel_instance.setBus(&_bus_instance);  // Set bus to panel
      }

      { // Configure display panel settings
        auto cfg = _panel_instance.config();  // Get panel config structure

        cfg.pin_cs = 40;    // Pin connected to CS (-1 = disable)
        cfg.pin_rst = 2;    // Pin connected to RST (-1 = disable)
        cfg.pin_busy = -1;  // Pin connected to BUSY (-1 = disable)

        // The following default values are set for each panel, including the BUSY pin (-1 = disable). If unsure, try commenting them out.

        cfg.memory_width = 320;    // Max width supported by controller IC
        cfg.memory_height = 480;   // Max height supported by controller IC
        cfg.panel_width = 320;     // Actual displayable width
        cfg.panel_height = 480;    // Actual displayable height
        cfg.offset_x = 0;          // Offset in X direction
        cfg.offset_y = 0;          // Offset in Y direction
        cfg.offset_rotation = 3;   // Offset value for rotation direction (0~7, 4~7 = inverted)
        cfg.dummy_read_pixel = 8;  // Number of dummy bits before reading pixel
        cfg.dummy_read_bits = 1;   // Number of dummy bits before reading non-pixel data
        cfg.readable = false;      // Set to true if panel supports data read
        cfg.invert = true;         // Set to true if brightness is inverted
        cfg.rgb_order = false;     // Set to true if red and blue are swapped
        cfg.dlen_16bit = false;    // Set to true if panel uses 16-bit data length
        cfg.bus_shared = true;     // Set to true if bus is shared with SD card (required for drawJpgFile, etc.)

        _panel_instance.config(cfg);
      }

      // { // Configure touch screen settings (remove if not used)
      //   auto cfg = _touch_instance.config();

      //   cfg.x_min = 0;            // Minimum X value from touch screen (raw)
      //   cfg.x_max = 319;          // Maximum X value from touch screen (raw)
      //   cfg.y_min = 0;            // Minimum Y value from touch screen (raw)
      //   cfg.y_max = 479;          // Maximum Y value from touch screen (raw)
      //   cfg.pin_int = 47;         // Pin connected to INT
      //   cfg.bus_shared = false;   // Set to true if bus is shared with display
      //   cfg.offset_rotation = 0;  // Adjust this if touch and display orientations don't match (0 ~ 7)

      //   // For I2C connection
      //   cfg.i2c_port = 0;         // Select I2C port (0 or 1)
      //   // cfg.i2c_addr = 0x38;    // I2C device address
      //   cfg.i2c_addr = 0x14;      // I2C device address
      //   cfg.pin_sda = 15;         // Pin connected to SDA
      //   cfg.pin_scl = 16;         // Pin connected to SCL
      //   cfg.freq = 400000;        // I2C clock speed

      //   _touch_instance.config(cfg);
      //   _panel_instance.setTouch(&_touch_instance);  // Attach touch to panel
      // }

      {
        auto cfg = _touch_instance.config();
        cfg.x_min = 0;
        cfg.x_max = 319;
        cfg.y_min = 0;
        cfg.y_max = 479;
        cfg.pin_int = 47;
        cfg.bus_shared = false;
        cfg.offset_rotation = 0;
        // I2C connection
        cfg.i2c_port = I2C_NUM_0;
        cfg.pin_sda = GPIO_NUM_15;
        cfg.pin_scl = GPIO_NUM_16;
        cfg.pin_rst = 48;
        cfg.freq = 400000;
        cfg.i2c_addr = 0x14;  // 0x5D , 0x14
        _touch_instance.config(cfg);
        _panel_instance.setTouch(&_touch_instance);
      }

      setPanel(&_panel_instance);
    }
};
