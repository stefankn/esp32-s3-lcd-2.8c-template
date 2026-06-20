#pragma once
#include <stdint.h>

// GT911 capacitive touch driver.
// I2C address 0x5D, confirmed via boot-time scan.
// SDA=GPIO15, SCL=GPIO7 (shared bus with TCA9554, QMI8658C, PCF85063, etc.)

void Touch_Init();
bool Touch_GetPoint(int16_t *x, int16_t *y);  // returns true while finger is down
