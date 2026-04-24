#ifndef LGFX_SETUP_H
#define LGFX_SETUP_H

#include <LovyanGFX.hpp>
#include <driver/i2c.h>

class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9488     _panel_instance;
    lgfx::Bus_SPI       _bus_instance;   


  public:
    LGFX(void) {
      {
        auto cfg = _bus_instance.config();

        // SPI bus configuration
        cfg.spi_host = SPI2_HOST;  // Select the SPI to use. For ESP32-S2, C3: SPI2_HOST or SPI3_HOST / For ESP32: VSPI_HOST or HSPI_HOST
        // With the upgrade of the ESP-IDF version, the descriptions of VSPI_HOST and HSPI_HOST have been deprecated. If an error occurs, please use SPI2_HOST and SPI3_HOST instead.
        cfg.spi_mode = 0;                     // Set the SPI communication mode (0 ~ 3)
        cfg.freq_write = 40000000;            // SPI clock during transmission (maximum 80MHz, rounded to a value obtained by dividing 80MHz by an integer)
        cfg.freq_read = 16000000;             // SPI clock during reception
        cfg.spi_3wire = false;                // Set to true if receiving via the MOSI pin
        cfg.use_lock = true;                  // Set to true when using the transaction lock
        cfg.dma_channel = SPI_DMA_CH_AUTO;    // Set the DMA channel to use (0 = no DMA used / 1 = 1ch / 2 = 2ch / SPI_DMA_CH_AUTO = automatic setting)
        // With the upgrade of the ESP-IDF version, it is now recommended to use SPI_DMA_CH_AUTO (automatic setting) as the DMA channel.
        cfg.pin_sclk = 42;                    // Set the pin number of the SPI SCLK
        cfg.pin_mosi = 39;                    // Set the pin number of the SPI MOSI
        cfg.pin_miso = -1;                    // Set the pin number of the SPI MISO (-1 = disable)
        cfg.pin_dc = 41;                      // Set the pin number of the SPI DC (-1 = disable)

        _bus_instance.config(cfg);               // Reflect the settings on the bus.
        _panel_instance.setBus(&_bus_instance);  // Set the bus on the panel.
      }

     { // Configure the display panel control settings.
        auto cfg = _panel_instance.config();  // Get the display panel configuration structure.

        cfg.pin_cs = 40;    // Pin number connected to CS. (-1 = disable)
        cfg.pin_rst = 2;    // Pin number connected to RST. (-1 = disable)
        cfg.pin_busy = -1;  // Pin number connected to BUSY. (-1 = disable)

        // The following default values are set for each panel, as well as the pin number connected to BUSY (-1 = disable). So if you're unsure about a certain item, you can comment it out and give it a try.

        cfg.memory_width = 320;    // Maximum width supported by the driver IC
        cfg.memory_height = 480;   // Maximum height supported by the driver IC
        cfg.panel_width = 320;     // Actual displayable width
        cfg.panel_height = 480;    // Actual displayable height
        cfg.offset_x = 0;          // Offset in the X direction of the panel
        cfg.offset_y = 0;          // Offset in the Y direction of the panel
        cfg.offset_rotation = 3;   // Offset value in the rotation direction, 0~7 (4~7 are inverted)
        cfg.dummy_read_pixel = 8;  // Number of dummy bits to read before reading a pixel
        cfg.dummy_read_bits = 1;   // Number of dummy read bits before reading data other than pixels
        cfg.readable = false;      // Set to true if data can be read
        cfg.invert = true;         // Set to true if the brightness of the panel is inverted
        cfg.rgb_order = false;     // Set to true if the red and blue colors of the panel are swapped
        cfg.dlen_16bit = false;    // Set to true for panels that send the data length in 16-bit units
        cfg.bus_shared = true;     // Set to true if the bus is shared with an SD card (perform bus control when using functions like drawJpgFile)

        _panel_instance.config(cfg);
      }

      setPanel(&_panel_instance);
    }
};

extern LGFX TFT;  // Use the display variable in the main file
#endif