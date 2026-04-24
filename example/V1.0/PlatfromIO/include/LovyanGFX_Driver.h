#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#include <driver/i2c.h>

class LGFX : public lgfx::LGFX_Device
{

    lgfx::Panel_ILI9488     _panel_instance;
    lgfx::Bus_SPI       _bus_instance;   
    lgfx::Touch_GT911  _touch_instance;

  public:
    LGFX(void) {
      {
        auto cfg = _bus_instance.config();

        cfg.spi_host = SPI2_HOST;  
        cfg.spi_mode = 0;                    
        cfg.freq_write = 40000000;            
        cfg.freq_read = 16000000;             
        cfg.spi_3wire = false;                
        cfg.use_lock = true;                
        cfg.dma_channel = SPI_DMA_CH_AUTO;   
        cfg.pin_sclk = 42;                    
        cfg.pin_mosi = 39;                  
        cfg.pin_miso = -1;                    
        cfg.pin_dc = 41;

        _bus_instance.config(cfg);              
        _panel_instance.setBus(&_bus_instance);  
      }

      { 
        auto cfg = _panel_instance.config();  

        cfg.pin_cs = 40;   
        cfg.pin_rst = 2;  
        cfg.pin_busy = -1; 

        cfg.memory_width = 320;    
        cfg.memory_height = 480;  
        cfg.panel_width = 320;    
        cfg.panel_height = 480;   
        cfg.offset_x = 0;         
        cfg.offset_y = 0;        
        cfg.offset_rotation = 3;  
        cfg.dummy_read_pixel = 8;  
        cfg.dummy_read_bits = 1;   
        cfg.readable = false;     
        cfg.invert = true;        
        cfg.rgb_order = false;     
        cfg.dlen_16bit = false;   
        cfg.bus_shared = true;   

        _panel_instance.config(cfg);
      }

      {
        auto cfg = _touch_instance.config();
        cfg.x_min = 0;
        cfg.x_max = 319;
        cfg.y_min = 0;
        cfg.y_max = 479;
        cfg.pin_int = 47;
        cfg.bus_shared = false;
        cfg.offset_rotation = 0;
        cfg.i2c_port = I2C_NUM_0;
        cfg.pin_sda = GPIO_NUM_15;
        cfg.pin_scl = GPIO_NUM_16;
        cfg.pin_rst = 48;
        cfg.freq = 400000;
        cfg.i2c_addr = 0x14; 
        _touch_instance.config(cfg);
        _panel_instance.setTouch(&_touch_instance);
      }

      setPanel(&_panel_instance);
    }
};
LGFX gfx;
