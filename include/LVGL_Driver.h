/*
The glue layer between LVGL and the physical display hardware. It does three things:

1. Sets up LVGL
Lvgl_Init() initializes the LVGL library, creates a draw buffer in memory, and registers the display driver so LVGL knows how to output pixels.

2. Connects LVGL to the hardware
Lvgl_Display_LCD() is the callback LVGL calls whenever it has finished rendering a frame. This function takes the rendered pixel buffer and passes it to Display_ST7701 to actually push it to the screen.

3. Keeps LVGL running
Lvgl_Loop() is called in the main loop() and drives the LVGL task scheduler — handling animations, timers, and UI updates. Without this being called regularly, the UI would freeze.

*/

#pragma once

#include <lvgl.h>
#include "lv_conf.h"
#include <esp_heap_caps.h>
#include "Display_ST7701.h"

#define LVGL_WIDTH     ESP_PANEL_LCD_WIDTH
#define LVGL_HEIGHT    ESP_PANEL_LCD_HEIGHT
#define LVGL_BUF_LEN  (LVGL_WIDTH * LVGL_HEIGHT * sizeof(lv_color_t))

#define EXAMPLE_LVGL_TICK_PERIOD_MS  2


extern lv_disp_drv_t disp_drv;

void Lvgl_print(const char * buf);
void Lvgl_Display_LCD( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p ); // Displays LVGL content on the LCD.    This function implements associating LVGL data to the LCD screen
void Lvgl_Touchpad_Read( lv_indev_drv_t * indev_drv, lv_indev_data_t * data );                // Read the touchpad
void example_increase_lvgl_tick(void *arg);

void Lvgl_Init(void);
void Lvgl_Loop(void);
