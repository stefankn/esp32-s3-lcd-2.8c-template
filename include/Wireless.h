/*
WiFi and Bluetooth utilities for the ESP32-S3-LCD-2.8C board.

Each function spawns a background FreeRTOS task on core 0 and returns immediately,
so the display and LVGL loop on core 1 are unaffected.

  Wifi_Scan()                   — scan for WiFi networks, log SSIDs/RSSI, store count in WIFI_NUM
  Wifi_Connect(ssid, password)  — connect to a secured network; sets WIFI_Connection on success,
                                  then checks internet reachability and sets INTERNET_Connection
  Bluetooth_Scan()              — run a 5-second BLE scan, log device count, store count in BLE_NUM
*/

#pragma once
#include <stdint.h>

extern bool     WIFI_Connection;
extern bool     INTERNET_Connection;
extern uint8_t  WIFI_NUM;
extern uint8_t  BLE_NUM;

void Wifi_Scan(void);
void Wifi_Connect(const char *ssid, const char *password);
void Bluetooth_Scan(void);
