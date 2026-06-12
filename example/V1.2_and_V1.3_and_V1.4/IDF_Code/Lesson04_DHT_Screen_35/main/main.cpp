#include "Arduino.h"
#include "LovyanGFX_Driver.h"
#include <Wire.h>
#include <SPI.h>
#include "ui.h"
#include "lvgl.h"
#include <Crowbits_DHT20.h> 

LGFX gfx; // Ensure global display instance
Crowbits_DHT20 dht20;

/* LVGL configuration */
#define HOR_RES 320
#define VER_RES 480
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf_1[HOR_RES * 20];
static lv_color_t buf_2[HOR_RES * 20];

/* Display flush callback */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
    
    gfx.startWrite();
    gfx.setAddrWindow(area->x1, area->y1, w, h);
    gfx.pushPixels((uint16_t*)color_p, w * h, true);
    gfx.endWrite();
    
    lv_disp_flush_ready(disp);
}

/* Touch input handling */
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

void set_dht_lvgl(){
    char DHT_buffer[6];
    int temp = (int)dht20.getTemperature();
    int humi = (int)dht20.getHumidity();

    snprintf(DHT_buffer, sizeof(DHT_buffer), "%d", temp);
    lv_label_set_text(ui_TempLabel1, DHT_buffer);

    memset(DHT_buffer, 0, sizeof(DHT_buffer));

    snprintf(DHT_buffer, sizeof(DHT_buffer), "%d", humi);
    lv_label_set_text(ui_HumiLabel2, DHT_buffer);
}

void setup() {
    Serial.begin(115200);
    while(!Serial); // Wait for serial connection
    Serial.println("\n\nSystem starting...");

    Wire.begin(15, 16);// Pin of IIC
    delay(50);

    dht20.begin();

    // Hardware initialization
    pinMode(38, OUTPUT);
    digitalWrite(38, LOW); // Initially turn off backlight

    // Display initialization
    gfx.begin();
    gfx.setRotation(2); // Landscape orientation: try values 0–3
    gfx.setBrightness(128);
    digitalWrite(38, HIGH);

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

    // UI initialization
    ui_init();

    Serial.println("[System] Initialization complete");
}

void loop() {
    set_dht_lvgl();
    lv_timer_handler(); // LVGL task handler
    delay(5);
}

