#include "LVGL_Driver.h"

void setup() {
  Serial.begin(115200);
  delay(1000);  // wait for serial to connect

  Serial.println();
  Serial.println("Setup starting");

  // Initialize the I2C bus on pins SDA=15, SCL=7
  I2C_Init();

  // Wait for the I2C bus and connected chips to stabilize
  delay(120);

  // Initialize the GPIO expander chip (all pins set as outputs)
  TCA9554PWR_Init(0x00);

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

  // Set background to black
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

  // Create a simple label
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Hello");
  lv_obj_set_style_text_color(label, lv_color_white(), 0);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_48, 0);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  Serial.println("Setup completed");
}

// Update the UI at roughly 200 times per second (1000ms / 5ms = 200)
void loop() {
  Lvgl_Loop();
  delay(5);
}
