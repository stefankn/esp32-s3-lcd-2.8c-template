#include "Clock.h"
#include "TCA9554PWR.h"
#include "Wireless.h"
#include <Arduino.h>
#include <time.h>

#if __has_include("config.h")
#include "config.h"
#endif

struct ClockLabels {
  lv_obj_t *hours;
  lv_obj_t *colon;
  lv_obj_t *minutes;
};

static void beep_task(void *param) {
  int count = (int)param;
  for (int i = 0; i < count; i++) {
    Set_EXIO(EXIO_PIN8, High);
    vTaskDelay(pdMS_TO_TICKS(150));
    Set_EXIO(EXIO_PIN8, Low);
    if (i < count - 1) {
      vTaskDelay(pdMS_TO_TICKS(150));  // gap between beeps
    }
  }
  vTaskDelete(NULL);
}

static void time_timer_cb(lv_timer_t *timer) {
  ClockLabels *labels = (ClockLabels *)timer->user_data;
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char buf[3];

    strftime(buf, sizeof(buf), "%H", &timeinfo);
    lv_label_set_text(labels->hours, buf);

    strftime(buf, sizeof(buf), "%M", &timeinfo);
    lv_label_set_text(labels->minutes, buf);

    // Blink the colon every second
    lv_obj_set_style_text_opa(labels->colon,
      (timeinfo.tm_sec % 2 == 0) ? LV_OPA_COVER : LV_OPA_TRANSP, 0);

    if (timeinfo.tm_sec == 0) {
      int beeps = 0;
      if (timeinfo.tm_min == 0)  beeps = 2;  // full hour: double beep
      if (timeinfo.tm_min == 30) beeps = 1;  // half hour: single beep
      if (beeps > 0) {
        xTaskCreatePinnedToCore(beep_task, "beep", 4096, (void *)beeps, 1, NULL, 0);
      }
    }
  }
}

// Waits for WiFi to connect before starting NTP, avoiding a race condition
// where configTime() and the WiFi stack both try to init the lwIP socket layer
static void ntp_start_cb(lv_timer_t *timer) {
  if (WIFI_Connection) {
#ifdef TIMEZONE
    configTzTime(TIMEZONE, "pool.ntp.org");
#else
    configTime(0, 0, "pool.ntp.org");
#endif
    lv_timer_del(timer);
  }
}

void Clock_Init(lv_obj_t *parent, const lv_font_t *font, lv_coord_t y) {
  ClockLabels *labels = new ClockLabels();

  auto make_label = [&](const char *text) {
    lv_obj_t *lbl = lv_label_create(parent);
    lv_label_set_text(lbl, text);
    lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl, font, 0);
    return lbl;
  };

  labels->hours   = make_label("--");
  labels->colon   = make_label(":");
  labels->minutes = make_label("--");

  // Lay out hours, colon, minutes in a horizontal row centered at y
  lv_obj_t *row = lv_obj_create(parent);
  lv_obj_remove_style_all(row);
  lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_layout(row, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(row, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_align(row, LV_ALIGN_TOP_MID, 0, y);

  // Re-parent labels into the row
  lv_obj_set_parent(labels->hours,   row);
  lv_obj_set_parent(labels->colon,   row);
  lv_obj_set_parent(labels->minutes, row);

  lv_timer_create(time_timer_cb, 1000, labels);
  lv_timer_create(ntp_start_cb, 500, NULL);
}
