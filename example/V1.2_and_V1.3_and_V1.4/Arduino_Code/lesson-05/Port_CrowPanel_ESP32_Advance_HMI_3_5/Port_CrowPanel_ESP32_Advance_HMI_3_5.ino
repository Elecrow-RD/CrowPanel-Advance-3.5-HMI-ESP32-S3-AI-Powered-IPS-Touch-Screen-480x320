#include "pins_config.h"
#include "LovyanGFX_Driver.h"
#include <Arduino.h>
#include <lvgl.h>
#include <Wire.h>
#include <SPI.h>
#include <esp_heap_caps.h>
#include <stdbool.h>
#include <Crowbits_DHT20.h>
#include "ui.h"

// Global display driver instance used by LVGL callbacks.
LGFX gfx;

// DHT20 sensor object used to read temperature and humidity over I2C.
Crowbits_DHT20 dht20;

// Minimum interval between UI sensor updates.
static constexpr uint32_t SENSOR_UPDATE_INTERVAL_MS = 200;

/*---------------------------------------------------------------
 * LVGL draw buffers
 * LVGL renders the UI here before my_disp_flush() transfers pixels to
 * the LCD panel.
 *--------------------------------------------------------------*/
static lv_color_t *buf;
static lv_color_t *buf1;

/**
 * @brief Copy rendered LVGL pixels to the LCD.
 *
 * This callback connects LVGL's drawing engine to LovyanGFX. When the
 * UI changes, LVGL provides a rectangle of RGB565 pixels, and the
 * callback pushes that rectangle to the display with DMA.
 *
 * @param disp LVGL display object that requested the refresh.
 * @param area Rectangle that needs to be updated.
 * @param px_map Pointer to rendered pixel data.
 * @return None.
 * @note Called by LVGL from lv_timer_handler().
 */
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
  if (gfx.getStartCount() > 0) {
    gfx.endWrite();
  }
  gfx.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::rgb565_t *)px_map);
  lv_display_flush_ready(disp);
}

#include "touch.h"

// Stores the most recent touch coordinate reported by the panel.
uint16_t touchX, touchY;

/**
 * @brief Provide touch state and coordinates to LVGL.
 *
 * LVGL treats the panel as a pointer input device. This function marks
 * the pointer as pressed only when LovyanGFX reports a valid touch
 * coordinate.
 *
 * @param indev LVGL input device object.
 * @param data Output structure filled with pointer state.
 * @return None.
 * @note Called by LVGL from lv_timer_handler().
 */
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
  data->state = LV_INDEV_STATE_REL;
  if ( gfx.getTouch( &touchX, &touchY ) ) {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = touchX;
    data->point.y = touchY;

    Serial.print( "Data x " );
    Serial.println( data->point.x );
    Serial.print( "Data y " );
    Serial.println( data->point.y );
  }
}

// GPIO that lights when the temperature crosses the lesson threshold.
const int ledPin = 18;

/**
 * @brief Initialize sensor, display, touch input, LVGL, and UI objects.
 *
 * The I2C bus is started before the DHT20 sensor, then the LCD and
 * LVGL runtime are prepared. ui_init() must run after the display is
 * registered so the generated labels can be drawn and updated.
 *
 * @param None.
 * @return None.
 * @note Called once by the Arduino runtime after reset.
 */
void setup()
{
  Serial.begin(115200);

  pinMode(18, OUTPUT);
  
  Wire.begin(15, 16);
  delay(50);

  dht20.begin();

  /*---------------------------------------------------------------
   * Initialize the LCD and LVGL display pipeline.
   * The full-screen buffers are allocated in PSRAM because this UI
   * refresh path needs more memory than small internal buffers provide.
   *--------------------------------------------------------------*/
  gfx.init();
  gfx.initDMA();
  gfx.startWrite();
  gfx.fillScreen(TFT_BLACK);

  lv_init();
  lv_tick_set_cb(millis);
  size_t buffer_size = sizeof(lv_color_t) * LCD_H_RES * LCD_V_RES;
  buf = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
  buf1 = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);

  lv_display_t *disp = lv_display_create(LCD_H_RES, LCD_V_RES);
  lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
  lv_display_set_buffers(disp, buf, buf1, buffer_size, LV_DISPLAY_RENDER_MODE_FULL);
  lv_display_set_flush_cb(disp, my_disp_flush);

  /*---------------------------------------------------------------
   * Register touch input and initialize the GT911 controller.
   * Without this input device, the UI can still display sensor values
   * but touch widgets will not respond.
   *--------------------------------------------------------------*/
  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);

  uint8_t gt911_address;
  delay(100);

  pinMode(38, OUTPUT);
  digitalWrite(38, HIGH);

  gt911_address = 0x14;
  touch_init(gt911_address);
  gfx.fillScreen(TFT_BLACK);
  ui_init();
  Serial.println( "Setup done" );
}

/**
 * @brief Refresh sensor values and keep the LVGL interface running.
 *
 * Temperature and humidity are sampled periodically and written only
 * when a value changes. This reduces unnecessary label redraws while
 * keeping the screen responsive.
 *
 * @param None.
 * @return None.
 * @note Called repeatedly by the Arduino runtime.
 */
void loop()
{
  lv_timer_handler();

  static uint32_t last_sensor_update = 0;
  static int last_temp = 0;
  static int last_humi = 0;
  static bool first_update = true;
  uint32_t now = millis();

  if (first_update || now - last_sensor_update >= SENSOR_UPDATE_INTERVAL_MS) {
    last_sensor_update = now;

    /*---------------------------------------------------------------
     * Read sensor data and update only changed labels.
     * The first pass always writes both labels so the screen starts
     * with real values instead of the design-time placeholder text.
     *--------------------------------------------------------------*/
    int temp = dht20.getTemperature();
    int humi = dht20.getHumidity();
    char DHT_buffer[6];

    if (first_update || temp != last_temp) {
      snprintf(DHT_buffer, sizeof(DHT_buffer), "%d", temp);
      lv_label_set_text(ui_TempLabel1, DHT_buffer);
      last_temp = temp;
    }

    if (first_update || humi != last_humi) {
      snprintf(DHT_buffer, sizeof(DHT_buffer), "%d", humi);
      lv_label_set_text(ui_HumiLabel2, DHT_buffer);
      last_humi = humi;
    }

    // The lamp output gives a visible hardware response when the
    // measured temperature exceeds the lesson threshold.
    digitalWrite(ledPin, temp > 30 ? HIGH : LOW);
    first_update = false;
  }

  delay(5);
}
