#define LGFX_USE_V1                     // Enable LovyanGFX V1 API
#include <LovyanGFX.hpp>                // Include LovyanGFX main header
#include <driver/i2c.h>                 // ESP-IDF I2C driver definitions

// Custom LGFX class inheriting from LovyanGFX device base class
class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9488 _panel_instance;   // ILI9488 LCD panel instance
    lgfx::Bus_SPI _bus_instance;           // SPI bus instance
    lgfx::Touch_GT911 _touch_instance;     // GT911 capacitive touch controller instance

  public:
    LGFX(void) {                           // LGFX constructor
      {
        auto cfg = _bus_instance.config(); // Get SPI bus configuration structure

        // SPI bus configuration
        cfg.spi_host = SPI2_HOST;          // Select SPI host (SPI2_HOST or SPI3_HOST for ESP32-S3)
        // With newer ESP-IDF versions, VSPI_HOST / HSPI_HOST are deprecated.
        // Use SPI2_HOST / SPI3_HOST instead if errors occur.

        cfg.spi_mode = 0;                  // SPI communication mode (0 ~ 3)
        cfg.freq_write = 40000000;         // SPI write clock frequency (max 80 MHz)
        cfg.freq_read = 16000000;          // SPI read clock frequency
        cfg.spi_3wire = false;             // Set true if receiving data via MOSI pin
        cfg.use_lock = true;               // Enable SPI transaction lock
        cfg.dma_channel = SPI_DMA_CH_AUTO; // Automatically select DMA channel
        // SPI_DMA_CH_AUTO is recommended in newer ESP-IDF versions

        cfg.pin_sclk = 42;                 // SPI SCLK pin number
        cfg.pin_mosi = 39;                 // SPI MOSI pin number
        cfg.pin_miso = -1;                 // SPI MISO pin number (-1 = disabled)
        cfg.pin_dc = 41;                   // Data/Command (DC) pin number (-1 = disabled)

        _bus_instance.config(cfg);         // Apply configuration to SPI bus
        _panel_instance.setBus(&_bus_instance); // Attach SPI bus to the panel
      }

      { // Configure display panel control settings
        auto cfg = _panel_instance.config(); // Get panel configuration structure

        cfg.pin_cs = 40;                   // Chip Select (CS) pin number (-1 = disabled)
        cfg.pin_rst = 2;                   // Reset (RST) pin number (-1 = disabled)
        cfg.pin_busy = -1;                 // Busy pin number (-1 = disabled)

        // The following values are default for most panels.
        // If unsure, try commenting them out and test.
        cfg.memory_width = 320;            // Maximum width supported by the panel IC
        cfg.memory_height = 480;           // Maximum height supported by the panel IC
        cfg.panel_width = 320;             // Actual visible panel width
        cfg.panel_height = 480;            // Actual visible panel height
        cfg.offset_x = 0;                  // X-axis display offset
        cfg.offset_y = 0;                  // Y-axis display offset
        cfg.offset_rotation = 3;           // Rotation offset (0~7, 4~7 are inverted)
        cfg.dummy_read_pixel = 8;           // Dummy bits before pixel read
        cfg.dummy_read_bits = 1;            // Dummy bits before non-pixel read
        cfg.readable = false;              // Set true if panel supports read-back
        cfg.invert = true;                 // Set true if display colors are inverted
        cfg.rgb_order = false;             // Set true if red/blue channels are swapped
        cfg.dlen_16bit = false;            // True if panel uses 16-bit data length units
        cfg.bus_shared = true;              // True if SPI bus is shared (e.g., SD card)

        _panel_instance.config(cfg);       // Apply panel configuration
      }

      {
        auto cfg = _touch_instance.config(); // Get touch controller configuration

        cfg.x_min = 0;                     // Minimum raw X value
        cfg.x_max = 319;                   // Maximum raw X value
        cfg.y_min = 0;                     // Minimum raw Y value
        cfg.y_max = 479;                   // Maximum raw Y value
        cfg.pin_int = 47;                  // Touch interrupt pin
        cfg.bus_shared = false;            // Touch does not share SPI bus
        cfg.offset_rotation = 0;           // No rotation offset

        // I2C connection settings
        cfg.i2c_port = I2C_NUM_0;           // Use I2C port 0
        cfg.pin_sda = GPIO_NUM_15;          // SDA pin
        cfg.pin_scl = GPIO_NUM_16;          // SCL pin
        cfg.pin_rst = 48;                   // Touch reset pin
        cfg.freq = 400000;                  // I2C clock frequency
        cfg.i2c_addr = 0x14;                // GT911 I2C address (0x5D or 0x14)

        _touch_instance.config(cfg);        // Apply touch configuration
        _panel_instance.setTouch(&_touch_instance); // Bind touch to display panel
      }

      setPanel(&_panel_instance);           // Set this panel as the active display
    }
};
