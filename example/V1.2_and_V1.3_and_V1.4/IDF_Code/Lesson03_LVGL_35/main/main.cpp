#include "Arduino.h"
#include "LovyanGFX_Driver.h"
#include <Wire.h>
#include <SPI.h>
#include "ui.h"
#include "lvgl.h"
#include "touch.h"

LGFX gfx; // Ensure that the instance is displayed globally

/* LVGL configuration */
#define HOR_RES 320
#define VER_RES 480

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf_1[HOR_RES * 20];
static lv_color_t buf_2[HOR_RES * 20];

/* Displays refresh callbacks */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
    
    gfx.startWrite();
    gfx.setAddrWindow(area->x1, area->y1, w, h);
    gfx.pushPixels((uint16_t*)color_p, w * h, true);
    gfx.endWrite();
    
    lv_disp_flush_ready(disp);
}

/* Touch input processing */

uint16_t touchX, touchY;
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
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

void setup() {
    Serial.begin(115200);
    while(!Serial); // Waiting for serial port connection
    Serial.println("\n\nSystem starting...");

    pinMode(18, OUTPUT);

    // hardware initialization
    pinMode(38, OUTPUT);
    digitalWrite(38, LOW); // Turn off the backlight initially

    // Display initialization
    gfx.begin();
    gfx.setRotation(0); // Landscape orientation: 0-3 Try different values
    gfx.setBrightness(128);
    
    digitalWrite(38, HIGH);

    // The I2C bus is initialized. Procedure
    Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
    Serial.println("[I2C] Bus started");

    // LVGL initialization
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf_1, buf_2, HOR_RES * 20);

    // Register display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = gfx.width();
    disp_drv.ver_res = gfx.height();
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // Register touch input
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_t * touch_indev = lv_indev_drv_register(&indev_drv);
    Serial.printf("[input] The touch device is registered(ID:%p)\n", touch_indev);

    uint8_t gt911_address = 0x14;
    touch_init(gt911_address);

    ui_init();
    Serial.println("[system] Initialization complete");
}

void loop() {
    lv_timer_handler(); // LVGL task processing
    delay(5);
}

