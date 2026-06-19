# CLAUDE.md — ESP32-S3-LCD-2.8C Template

## Project Overview

PlatformIO project that serves as a starter template for the **Waveshare ESP32-S3-LCD-2.8C** board. Drives a 480×480 ST7701 RGB LCD using the ESP32-S3's built-in RGB LCD peripheral, with LVGL 8.3.9 for the UI layer.

## Build System

- **Build tool**: PlatformIO CLI
- **Framework**: Arduino (entry point is `src/main.cpp`)
- **Config**: Copy `platformio.ini.example` to `platformio.ini` before building
- **Dependencies**: Managed by PlatformIO (`lvgl/lvgl@^8.3.0`)
- **PSRAM**: Enabled via `board_build.arduino.memory_type = qio_opi` in `platformio.ini`

### Common Commands

- Build & flash: `pio run -t upload`
- Clean build: `pio run -t clean`
- Serial monitor: `pio device monitor --baud 115200`

## Project Structure

```
platformio.ini.example           PlatformIO config template (copy to platformio.ini)
src/
  main.cpp                         Main sketch (setup/loop)
  Display_ST7701.cpp               ST7701 init over SPI + RGB panel driver + backlight PWM
  LVGL_Driver.cpp                  LVGL init, frame buffers, flush callback, tick timer
  I2C_Driver.cpp                   I2C bus (SDA=15, SCL=7)
  TCA9554PWR.cpp                   TCA9554 GPIO expander at I2C 0x20
include/
  Display_ST7701.h
  LVGL_Driver.h
  I2C_Driver.h
  TCA9554PWR.h
  lv_conf.h                        LVGL compile-time configuration
```

## Hardware Reference

The board schematic is at `docs/ESP32-S3-Touch-LCD-2.8C_schematic_diagram.pdf`. Consult it when you need to understand how components are wired, trace signal paths, or verify pin connections not listed below.

## Key Hardware Details

### Pin Assignments

| Function | GPIO |
|---|---|
| I2C SDA | 15 |
| I2C SCL | 7 |
| LCD SPI CLK | 2 |
| LCD SPI MOSI | 1 |
| RGB HSYNC | 38 |
| RGB VSYNC | 39 |
| RGB DE | 40 |
| RGB PCLK | 41 |
| RGB Data[0..15] | 5,45,48,47,21,14,13,12,11,10,9,46,3,8,18,17 |
| Backlight PWM | 6 |

### TCA9554 GPIO Expander (I2C 0x20)

| Pin | Function |
|---|---|
| EXIO_PIN1 | ST7701 Reset |
| EXIO_PIN3 | ST7701 Chip Select |
| EXIO_PIN8 | Backlight enable (note: schematic shows P7 wired to buzzer — may be a board revision difference) |

## Initialization Order

The order in `setup()` is critical — do not reorder:
1. Serial → I2C → TCA9554 init (all outputs)
2. Pull EXIO_PIN8 low (prevents backlight flicker during LCD init)
3. Backlight PWM init
4. Backlight set to 100%
5. ST7701 LCD controller init (SPI + ~62 config commands)
6. LVGL init (allocates dual PSRAM frame buffers, registers flush/tick)
7. Build initial UI

## Display & LVGL Configuration

- Resolution: 480×480, 16-bit RGB565
- Pixel clock: 30 MHz
- Single RGB panel frame buffer in PSRAM; dual LVGL draw buffers (each 480×480×2 bytes) also in PSRAM
- Bounce buffer: 10 × LCD_HEIGHT bytes (prevents screen drift artifacts)
- LVGL tick: 2 ms timer; `lv_timer_handler()` called every 5 ms from `loop()`

## Conventions

- Touch is stubbed — `LVGL_Driver` registers a dummy touchpad that always returns "released"
- Backlight is controlled via `Set_Backlight(0–100)` (percentage)
- Add new UI code in `setup()` after `Lvgl_Init()`, or factor it into a separate `.cpp` file
- Keep frame buffer allocations in PSRAM (`MALLOC_CAP_SPIRAM`) to avoid exhausting internal SRAM
