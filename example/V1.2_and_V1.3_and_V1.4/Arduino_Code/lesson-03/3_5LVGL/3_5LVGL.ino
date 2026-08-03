#include "pins_config.h"
#include "LovyanGFX_Driver.h"

#include <Arduino.h>
#include <lvgl.h>
#include <SPI.h>
#include <esp_heap_caps.h>

#include <stdbool.h>

#include "ui.h"

// Global display driver instance used by LVGL callbacks.
LGFX gfx;

// GPIO that drives the external lamp controlled by the UI.
static constexpr uint8_t LAMP_PIN = 18;

/**
 * @brief Set the lamp output from SquareLine/LVGL event code.
 *
 * This C-compatible wrapper can be called from generated UI event
 * files. Keeping it in the main sketch makes the hardware action easy
 * to find while the UI files remain focused on screen objects.
 *
 * @param enabled true to turn the lamp on, false to turn it off.
 * @return None.
 * @note Called when the UI event logic requests a lamp state change.
 */
extern "C" void set_lamp_state(bool enabled) {
  digitalWrite(LAMP_PIN, enabled ? HIGH : LOW);
  Serial.println(enabled ? "Lamp ON" : "Lamp OFF");
}

/*---------------------------------------------------------------
 * LVGL draw buffers
 * LVGL renders the interface into these buffers before the pixels are
 * sent to the LCD by the display flush callback.
 *--------------------------------------------------------------*/
static lv_color_t *buf;
static lv_color_t *buf1;

/**
 * @brief Copy a rendered LVGL area to the LCD.
 *
 * LVGL calls this function whenever part of the screen needs to be
 * refreshed. The callback transfers the RGB565 pixel block through
 * LovyanGFX DMA and then reports that the refresh is complete.
 *
 * @param disp LVGL display object that requested the refresh.
 * @param area Rectangle that contains the pixels to update.
 * @param px_map Pointer to the rendered pixel data.
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

// Stores the most recent touch coordinate reported by the display driver.
uint16_t touchX, touchY;

/**
 * @brief Provide touch coordinates to LVGL.
 *
 * LVGL reads pointer state through this callback. When the panel is
 * touched, the callback marks the input as pressed and passes the
 * current coordinate to LVGL so widgets can react to the finger.
 *
 * @param indev LVGL input device object.
 * @param data Output structure filled with touch state and position.
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

/**
 * @brief Initialize GPIO, LCD, LVGL, touch input, and the generated UI.
 *
 * The setup sequence first places the lamp in a known off state, then
 * prepares the display and LVGL runtime before the SquareLine UI is
 * created. The backlight is enabled after the display is ready so the
 * first visible screen is stable.
 *
 * @param None.
 * @return None.
 * @note Called once by the Arduino runtime after reset.
 */
void setup()
{
  Serial.begin(115200);

  pinMode(LAMP_PIN, OUTPUT);
  set_lamp_state(false);
  
  /*---------------------------------------------------------------
   * Initialize the LCD before registering it with LVGL.
   * DMA is enabled so full-screen UI refreshes can be transferred
   * efficiently to the panel.
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

  /*---------------------------------------------------------------
   * Register LCD output and touch input with LVGL.
   * The display callback moves pixels to the screen, while the input
   * callback converts raw touch coordinates into LVGL pointer events.
   *--------------------------------------------------------------*/
  lv_display_t *disp = lv_display_create(LCD_H_RES, LCD_V_RES);
  lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
  lv_display_set_buffers(disp, buf, buf1, buffer_size, LV_DISPLAY_RENDER_MODE_FULL);
  lv_display_set_flush_cb(disp, my_disp_flush);

  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);
  lv_indev_set_display(indev, disp);

  /*---------------------------------------------------------------
   * Show the generated user interface.
   * ui_init() creates the widgets exported by SquareLine Studio. If
   * this call is removed, the LCD may light up but the lesson UI will
   * not appear.
   *--------------------------------------------------------------*/
  pinMode(38, OUTPUT);
  digitalWrite(38, HIGH);

  gfx.fillScreen(TFT_BLACK);
  ui_init();

  Serial.println( "Setup done" );
}

/**
 * @brief Let LVGL process animations, input, and redraw work.
 *
 * The short delay yields CPU time while keeping the interface
 * responsive to touch events.
 *
 * @param None.
 * @return None.
 * @note Called repeatedly by the Arduino runtime.
 */
void loop()
{
  lv_timer_handler();
  delay(5);
}
