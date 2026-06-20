#include "Touch.h"
#include <Wire.h>
#include <Arduino.h>

// GT911 capacitive touch controller, confirmed at I2C address 0x5D via boot scan.
// Uses 16-bit register addresses with a stop-start sequence (matching the pattern
// used by I2C_Driver.cpp — endTransmission(true) then requestFrom).
//
// GT911 relevant registers:
//   0x814E  Point info: bit 7 = buffer-ready flag, bits 3:0 = touch point count
//   0x8150  Touch point 1: XL, XH, YL, YH, SizeL, SizeH, EventID, reserved (8 bytes)
//
// After reading, write 0x00 to 0x814E to clear the buffer-ready flag so the IC
// can fill it with the next sample.

#define GT911_ADDR       0x5D
#define GT911_STATUS_REG 0x814E
#define GT911_POINT1_REG 0x8150

static bool gt911_read(uint16_t reg, uint8_t *buf, uint8_t len) {
  Wire.beginTransmission(GT911_ADDR);
  Wire.write((uint8_t)(reg >> 8));
  Wire.write((uint8_t)(reg & 0xFF));
  if (Wire.endTransmission(true) != 0) return false;
  Wire.requestFrom((uint8_t)GT911_ADDR, len);
  for (uint8_t i = 0; i < len && Wire.available(); i++) {
    buf[i] = Wire.read();
  }
  return true;
}

static void gt911_clear_status() {
  Wire.beginTransmission(GT911_ADDR);
  Wire.write((uint8_t)(GT911_STATUS_REG >> 8));
  Wire.write((uint8_t)(GT911_STATUS_REG & 0xFF));
  Wire.write(0x00);
  Wire.endTransmission(true);
}

void Touch_Init() {
  // Clear any stale buffer-ready flag left over from before firmware started.
  gt911_clear_status();
}

bool Touch_GetPoint(int16_t *x, int16_t *y) {
  uint8_t status = 0;
  if (!gt911_read(GT911_STATUS_REG, &status, 1)) {
    return false;
  }

  // Buffer-ready bit not set — no new data from GT911 yet.
  if (!(status & 0x80)) {
    return false;
  }

  // Always clear status when buffer-ready is set, even if touch count is zero
  // (count=0 means finger-lifted event). Without this, GT911 cannot write the
  // next sample and all subsequent touches are dropped.
  uint8_t count = status & 0x0F;
  if (count == 0) {
    gt911_clear_status();
    return false;
  }

  uint8_t buf[4] = {0};
  bool ok = gt911_read(GT911_POINT1_REG, buf, 4);
  gt911_clear_status();
  if (!ok) return false;

  *x = (int16_t)(buf[0] | ((uint16_t)buf[1] << 8));
  *y = (int16_t)(buf[2] | ((uint16_t)buf[3] << 8));
  return true;
}
