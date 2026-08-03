#include "pins_config.h"
#include "LovyanGFX_Driver.h"

#include <Arduino.h>
#include <lvgl.h>
#include <SPI.h>
#include <esp_heap_caps.h>

#include "ui.h"

/*---------------------------------------------------------------
 * LVGL display state
 * The lesson keeps the screen object and draw buffers at file scope
 * so the callbacks can use them without extra wrapper code.
 *--------------------------------------------------------------*/
static lv_display_t *display;
static uint8_t *buf;
static uint8_t *buf1;

// Number of screen rows rendered per flush buffer.
static constexpr uint32_t DRAW_BUFFER_LINES = 20;

// GPIO that controls the external lamp.
static constexpr uint8_t LAMP_PIN = 18;

// Active levels used by the lamp helper.
static constexpr uint8_t LAMP_ON_LEVEL = HIGH;
static constexpr uint8_t LAMP_OFF_LEVEL = LOW;

/**
 * @brief Set the lamp output from the generated UI code.
 *
 * The UI event files call this wrapper so the lesson can switch the
 * GPIO without exposing platform-specific details in the generated UI
 * layer.
 *
 * @param enabled true to turn the lamp on, false to turn it off.
 * @return None.
 * @note Called by UI event callbacks.
 */
extern "C" void app_lamp_set(int enabled) {
  const uint8_t level = enabled ? LAMP_ON_LEVEL : LAMP_OFF_LEVEL;
  digitalWrite(LAMP_PIN, level);
  Serial.printf("Lamp %s (GPIO%u=%s)\n", enabled ? "ON" : "OFF", LAMP_PIN,
                level == HIGH ? "HIGH" : "LOW");
}

/**
 * @brief Copy one LVGL invalidated area to the LCD.
 *
 * LVGL calls this function when part of the screen needs refreshing.
 * The callback transfers the rendered RGB565 buffer to the panel and
 * then tells LVGL that the refresh has finished.
 *
 * @param disp LVGL display object.
 * @param area Rectangle that needs repainting.
 * @param color_p Pixel buffer produced by LVGL.
 * @return None.
 * @note Called by LVGL from lv_timer_handler().
 */
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *color_p) {
  if (gfx.getStartCount() > 0) {
    gfx.endWrite();
  }
  gfx.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1,
                   reinterpret_cast<const lgfx::rgb565_t *>(color_p));
  gfx.waitDMA();
  lv_display_flush_ready(disp);
}

// Stores the latest touch coordinate returned by LovyanGFX.
uint16_t touchX, touchY;

/**
 * @brief Report touch state to LVGL.
 *
 * The board exposes the display as a pointer device so the generated
 * UI can react to user input. When a valid touch is present, LVGL
 * receives the coordinates and marks the input as pressed.
 *
 * @param indev_driver LVGL input device object.
 * @param data Output structure filled with the pointer state.
 * @return None.
 * @note Called by LVGL from lv_timer_handler().
 */
void my_touchpad_read(lv_indev_t *indev_driver, lv_indev_data_t *data) {
  data->state = LV_INDEV_STATE_RELEASED;
  if ( gfx.getTouch( &touchX, &touchY ) ) {
    data->state = LV_INDEV_STATE_PRESSED;

    data->point.x = touchX;
    data->point.y = touchY;

    Serial.print( "Data x " );
    Serial.println( data->point.x );
    Serial.print( "Data y " );
    Serial.println( data->point.y );
  }
}

/**
 * @brief Return the current millisecond counter to LVGL.
 *
 * The tick callback lets LVGL measure animations and timeouts using
 * Arduino's millisecond clock.
 *
 * @param None.
 * @return Current time in milliseconds.
 */
static uint32_t my_tick_get(void) {
  return millis();
}

/**
 * @brief Initialize the board, LCD, LVGL, touch input, and UI.
 *
 * The setup order is important: GPIO first, then display, then LVGL,
 * then the generated UI. That sequence ensures the display is ready
 * before any widgets try to draw.
 *
 * @param None.
 * @return None.
 * @note Called once by the Arduino runtime after reset.
 */
void setup()
{
  Serial.begin(115200);
  pinMode(LAMP_PIN, OUTPUT);
  app_lamp_set(0);

  delay(50);

  // Init Display
  gfx.init();
  gfx.initDMA();
  gfx.startWrite();
  gfx.fillScreen(TFT_BLACK);

  lv_init();
  lv_tick_set_cb(my_tick_get);
  const size_t buffer_size = LCD_H_RES * DRAW_BUFFER_LINES *
                             lv_color_format_get_size(LV_COLOR_FORMAT_RGB565);
  buf = static_cast<uint8_t *>(heap_caps_malloc(buffer_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL));
  buf1 = static_cast<uint8_t *>(heap_caps_malloc(buffer_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL));

  if (buf == nullptr || buf1 == nullptr) {
    Serial.println("LVGL draw buffer allocation failed");
    while (true) {
      delay(1000);
    }
  }

  display = lv_display_create(LCD_H_RES, LCD_V_RES);
  lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
  lv_display_set_flush_cb(display, my_disp_flush);
  lv_display_set_buffers(display, buf, buf1, buffer_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

  /*---------------------------------------------------------------
   * Register the touch panel as an LVGL pointer device.
   * If the callback is removed, the UI can still render but widgets
   * will not react to fingers.
   *--------------------------------------------------------------*/
  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);

  // The backlight is enabled only after the display pipeline is ready.
  pinMode(38, OUTPUT);
  digitalWrite(38, HIGH);

  gfx.fillScreen(TFT_BLACK);
  ui_init();

  Serial.println( "Setup done" );
}

/**
 * @brief Run the LVGL task handler.
 *
 * LVGL needs frequent service calls to update animations, redraw the
 * screen, and process touch events.
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
