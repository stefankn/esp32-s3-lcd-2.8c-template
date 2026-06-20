#pragma once
#include <lvgl.h>

// Call after Lvgl_Init(). Creates hour, colon, and minute labels as children of
// parent, positioned horizontally centered at the given y offset from the top.
void Clock_Init(lv_obj_t *parent, const lv_font_t *font, lv_coord_t y);
