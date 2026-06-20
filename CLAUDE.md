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

- Build: `make build`
- Build & flash: `make run`
- Flash only: `make upload`
- Serial monitor: `make monitor`
- Regenerate compile_commands.json: `make compiledb`
- Clean build: `make clean`

## Project Structure

```
Makefile                         Common development commands (build, flash, monitor, etc.)
platformio.ini                   PlatformIO config (gitignored — copy from .example)
platformio.ini.example           PlatformIO config template
docs/
  ESP32-S3-Touch-LCD-2.8C_schematic_diagram.pdf
src/
  main.cpp                         Main sketch (setup/loop)
  Display_ST7701.cpp               ST7701 init over SPI + RGB panel driver + backlight PWM
  LVGL_Driver.cpp                  LVGL init, frame buffers, flush callback, tick timer
  I2C_Driver.cpp                   I2C bus (SDA=15, SCL=7)
  TCA9554PWR.cpp                   TCA9554 GPIO expander at I2C 0x20
  Wireless.cpp                     WiFi and Bluetooth scan utilities
  Clock.cpp                        NTP time sync, 24h clock display, hourly/half-hourly beeps
  sample_image.c                   Background image (LVGL C array, 480×480, CF_TRUE_COLOR)
  digital7_72.c                    Digital-7 font at 72px (converted for LVGL 8, digits + colon + hyphen)
include/
  Display_ST7701.h
  LVGL_Driver.h
  I2C_Driver.h
  TCA9554PWR.h
  Wireless.h                       WiFi/Bluetooth scan API
  Clock.h                          Clock_Init() — call after Lvgl_Init()
  lv_conf.h                        LVGL compile-time configuration
  secrets.h.example                WiFi credentials template (copy to secrets.h)
  secrets.h                        WiFi credentials (gitignored — do not commit)
  config.h.example                 User configuration template (copy to config.h)
  config.h                         User configuration — timezone (gitignored — do not commit)
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
| EXIO_PIN8 | Buzzer (active buzzer via NPN transistor Q5; High = on, Low = off) |

## Initialization Order

The order in `setup()` is critical — do not reorder:
1. Serial → I2C → TCA9554 init (all outputs)
2. Pull EXIO_PIN8 low (silences buzzer during LCD init)
3. Backlight PWM init
4. Backlight set to 100%
5. ST7701 LCD controller init (SPI + ~62 config commands)
6. LVGL init (allocates dual PSRAM frame buffers, registers flush/tick)
7. Build initial UI

## Display & LVGL Configuration

- Resolution: 480×480, 16-bit RGB565
- Pixel clock: 18 MHz (reduced from 30 MHz for stability under WiFi load)
- Single RGB panel frame buffer in PSRAM; dual LVGL draw buffers (each 480×480×2 bytes) also in PSRAM
- Bounce buffer: 10 × LCD_HEIGHT bytes (prevents screen drift artifacts)
- LVGL tick: 2 ms timer; `lv_timer_handler()` called every 5 ms from `loop()`
- Vsync sync: flush callback uses a semaphore pair (`sem_gui_ready` / `sem_vsync_end`) to wait for vsync before writing to the frame buffer — prevents display shift when WiFi is active

## Wireless

`Wifi_Scan()`, `Wifi_Connect(ssid, password)`, and `Bluetooth_Scan()` in `Wireless.cpp` each spawn an independent FreeRTOS task pinned to core 0. Call them after `setup()` completes — they return immediately and do not block the LVGL loop on core 1. Results are stored in `WIFI_NUM`, `BLE_NUM`, `WIFI_Connection`, and `INTERNET_Connection`.

After `Wifi_Connect()` establishes a WiFi association, it immediately performs an internet reachability check by opening a TCP connection to `8.8.8.8:53` (Google DNS, 2-second timeout). The result is stored in `INTERNET_Connection` and logged as `Internet: reachable` / `Internet: unreachable`. This distinguishes a working internet path from merely joining a local network.

WiFi credentials go in `include/secrets.h` (gitignored — copy from `include/secrets.h.example`). When present, `main.cpp` picks them up via `#if defined(WIFI_SSID) && defined(WIFI_PASSWORD)` and calls `Wifi_Connect()` automatically.

After WiFi connects, `Clock.cpp` starts NTP sync via `configTzTime()`. The timezone is configured in `include/config.h` (gitignored — copy from `include/config.h.example`) using a POSIX tz string (e.g. `"CET-1CEST,M3.5.0,M10.5.0/3"`). Without `config.h`, UTC is used as fallback.

## Conventions

- Touch is stubbed — `LVGL_Driver` registers a dummy touchpad that always returns "released"
- Backlight is controlled via `Set_Backlight(0–100)` (percentage)
- Buzzer is controlled via `Set_EXIO(EXIO_PIN8, High/Low)` — active buzzer, fixed tone, on/off only
- `Clock_Init(label)` starts the 1 s LVGL display timer and the NTP polling timer; call it after `Lvgl_Init()` and after creating the label widget
- Buzzer beep sequences run as short-lived FreeRTOS tasks on core 0 (via `xTaskCreatePinnedToCore`) to avoid blocking the LVGL loop on core 1
- Add new UI code in `setup()` after `Lvgl_Init()`, or factor it into a separate `.cpp` file
- Keep frame buffer allocations in PSRAM (`MALLOC_CAP_SPIRAM`) to avoid exhausting internal SRAM
- Custom fonts are pre-rendered C arrays placed in `src/`. Declare them in `lv_conf.h` via `LV_FONT_CUSTOM_DECLARE`. Fonts generated by the LVGL online converter target v9 — remove `.static_bitmap` and the `.fallback` block before building against LVGL 8
