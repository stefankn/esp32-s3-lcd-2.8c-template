# ESP32-S3-LCD-2.8C Template

A starter template for the [Waveshare ESP32-S3-LCD-2.8C](https://www.waveshare.com/esp32-s3-lcd-2.8c.htm) development board. Brings up the 480×480 ST7701 RGB display and initialises the LVGL 8 graphics library, ready for your application.

## Hardware

| | |
|---|---|
| **Board** | Waveshare ESP32-S3-LCD-2.8C |
| **MCU** | ESP32-S3 (dual-core Xtensa LX7, 240 MHz) |
| **Display** | 2.8-inch IPS, 480×480, ST7701 controller |
| **Interface** | 16-bit RGB565 parallel panel via ESP32-S3 RGB LCD peripheral |
| **GPIO expander** | TCA9554PWR (I2C, address 0x20) |
| **PSRAM** | Required — frame buffers are allocated from PSRAM |

## Features

- Full ST7701 initialisation sequence over SPI
- ESP32-S3 RGB LCD peripheral configured for 480×480 @ 30 MHz pixel clock
- LVGL 8.3.9 integrated with dual PSRAM frame buffers
- Backlight PWM control (`Set_Backlight(0–100)`)
- TCA9554 GPIO expander driver for display reset/CS/backlight enable
- "Hello" label as a minimal working UI example

## Requirements

### PlatformIO Setup

1. Install the [PlatformIO CLI](https://docs.platformio.org/en/latest/core/installation/index.html)
2. Copy `platformio.ini.example` to `platformio.ini`:
   ```sh
   cp platformio.ini.example platformio.ini
   ```
3. (Optional) Uncomment and set `upload_port` / `monitor_port` in `platformio.ini` if PlatformIO does not auto-detect your device

Dependencies are fetched automatically by PlatformIO — no manual library installation needed.

## Project Structure

```
platformio.ini.example           PlatformIO config template (copy to platformio.ini)
src/
  esp32-s3-lcd-2.8c-template.ino  Entry point — setup() / loop()
  Display_ST7701.cpp               ST7701 SPI init, RGB panel driver, backlight PWM
  LVGL_Driver.cpp                  LVGL init, frame buffers, flush callback, tick timer
  I2C_Driver.cpp                   I2C bus driver (SDA=15, SCL=7)
  TCA9554PWR.cpp                   TCA9554 8-bit GPIO expander driver
include/
  Display_ST7701.h
  LVGL_Driver.h
  I2C_Driver.h
  TCA9554PWR.h
  lv_conf.h                        LVGL compile-time configuration
```

## Pin Reference

### I2C

| Signal | GPIO |
|---|---|
| SDA | 15 |
| SCL | 7 |

### LCD SPI (ST7701 controller)

| Signal | GPIO |
|---|---|
| CLK | 2 |
| MOSI | 1 |

### RGB Panel (ESP32-S3 RGB peripheral)

| Signal | GPIO |
|---|---|
| HSYNC | 38 |
| VSYNC | 39 |
| DE | 40 |
| PCLK | 41 |
| D0–D15 | 5, 45, 48, 47, 21, 14, 13, 12, 11, 10, 9, 46, 3, 8, 18, 17 |

### Backlight

| Signal | GPIO |
|---|---|
| PWM | 6 |

## Getting Started

1. Clone or download this repository
2. Copy `platformio.ini.example` to `platformio.ini`
3. Build and flash: `pio run -t upload`
4. The display should show a black screen with a white "Hello" label

If you encounter build issues, clean the build directory with `pio run -t clean` and retry.

Monitor serial output with `pio device monitor --baud 115200`.

## Adding Your UI

Place your LVGL widget code in `setup()` after `Lvgl_Init()`:

```cpp
void setup() {
    // ... hardware init (do not reorder) ...
    Lvgl_Init();

    // Your UI code here
    lv_obj_t *btn = lv_btn_create(lv_scr_act());
    // ...
}
```

Keep large allocations in PSRAM using `heap_caps_malloc(size, MALLOC_CAP_SPIRAM)` to preserve internal SRAM for LVGL objects and stack.

## Notes

- **Touch**: Not implemented. The touchpad driver is registered as a stub that always returns "released". Wire up your own touch IC driver if needed.
- **Frame buffers**: Two full-screen LVGL draw buffers (480×480×2 bytes each) live in PSRAM. The RGB panel frame buffer is also in PSRAM.
- **Backlight**: Call `Set_Backlight(brightness)` with a value 0–100 at any time to adjust brightness.

## License

MIT
