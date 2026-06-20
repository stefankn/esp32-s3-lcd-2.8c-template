#include "Clock.h"
#include "TCA9554PWR.h"
#include "Wireless.h"
#include <Arduino.h>
#include <time.h>

#if __has_include("config.h")
#include "config.h"
#endif

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
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char buf[6];
    strftime(buf, sizeof(buf), "%H:%M", &timeinfo);
    lv_label_set_text((lv_obj_t *)timer->user_data, buf);

    if (timeinfo.tm_sec == 0) {
      int beeps = 0;
      if (timeinfo.tm_min == 0)  beeps = 2;  // full hour: double beep
      if (timeinfo.tm_min == 30) beeps = 1;  // half hour: single beep
      if (beeps > 0) {
        xTaskCreatePinnedToCore(beep_task, "beep", 1024, (void *)beeps, 1, NULL, 0);
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

void Clock_Init(lv_obj_t *label) {
  lv_timer_create(time_timer_cb, 1000, label);
  lv_timer_create(ntp_start_cb, 500, NULL);
}
