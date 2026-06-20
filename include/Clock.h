#pragma once
#include <lvgl.h>

// Call after Lvgl_Init() — starts the 1s time display timer and the NTP polling timer.
void Clock_Init(lv_obj_t *label);
