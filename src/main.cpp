#include "LVGL_Driver.h"
#include "Wireless.h"
#include "Clock.h"
#include "Touch.h"
#include <Arduino.h>

#if __has_include("secrets.h")
#include "secrets.h"
#endif

LV_IMG_DECLARE(cats);

static void touch_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_point_t p;
  lv_indev_get_point(lv_indev_get_act(), &p);
  switch (code) {
    case LV_EVENT_PRESSED:            Serial.printf("PRESSED      x=%d y=%d\n", p.x, p.y); break;
    case LV_EVENT_RELEASED:           Serial.printf("RELEASED     x=%d y=%d\n", p.x, p.y); break;
    case LV_EVENT_CLICKED:            Serial.printf("CLICKED      x=%d y=%d\n", p.x, p.y); break;
    case LV_EVENT_LONG_PRESSED:       Serial.printf("LONG_PRESSED x=%d y=%d\n", p.x, p.y); break;
    default: break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000); // wait for serial to connect

  Serial.println();
  Serial.println("Setup starting");

  // Initialize the I2C bus on pins SDA=15, SCL=7
  I2C_Init();

  // Wait for the I2C bus and connected chips to stabilize
  delay(120);

  // Initialize the GPIO expander chip (all pins set as outputs)
  TCA9554PWR_Init(0x00);

  Touch_Init();

  // Pull backlight enable pin low before initializing (avoid flicker)
  Set_EXIO(EXIO_PIN8, Low);

  // Initialize the PWM channel that controls backlight brightness
  Backlight_Init();

  // Set backlight to 100% brightness
  Set_Backlight(100);

  // Initialize the ST7701 LCD controller and RGB panel
  LCD_Init();

  // Initialize LVGL and connect it to the display hardware
  Lvgl_Init();

  lv_obj_add_event_cb(lv_scr_act(), touch_event_cb, LV_EVENT_ALL, NULL);

  // Set background to black
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

  // Display image as full-screen background
  lv_obj_t *img = lv_img_create(lv_scr_act());
  lv_img_set_src(img, &cats);
  lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

  Clock_Init(lv_scr_act(), &digital7_72, 75);

  // Start WiFi and Bluetooth scans as independent background tasks on core 0
  // Wifi_Scan();
  // Bluetooth_Scan();

#if defined(WIFI_SSID) && defined(WIFI_PASSWORD)
  Wifi_Connect(WIFI_SSID, WIFI_PASSWORD);
#endif

  Serial.println("Setup completed");
}

// Update the UI at roughly 200 times per second (1000ms / 5ms = 200)
void loop() {
  Lvgl_Loop();
  delay(5);
}
