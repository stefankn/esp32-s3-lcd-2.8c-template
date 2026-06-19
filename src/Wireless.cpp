#include "Wireless.h"
#include <Arduino.h>
#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEScan.h>

bool    WIFI_Connection = false;
uint8_t WIFI_NUM        = 0;
uint8_t BLE_NUM         = 0;

static void WifiScanTask(void *parameter)
{
  Serial.println();
  Serial.println("/**********WiFi Scan**********/");
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(true);
  int count = WiFi.scanNetworks();
  if (count == 0) {
    Serial.println("No WiFi networks found");
  } else {
    Serial.printf("Found %d WiFi network(s)\n", count);
    for (int i = 0; i < count; i++)
      Serial.printf("  [%d] %s (RSSI: %d dBm)\n", i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i));
  }
  WiFi.scanDelete();
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  vTaskDelay(pdMS_TO_TICKS(100));
  Serial.println("/**********WiFi Scan Over**********/\n");
  WIFI_NUM = count;
  vTaskDelete(NULL);
}

static void BluetoothScanTask(void *parameter)
{
  Serial.println();
  Serial.println("/**********Bluetooth Scan**********/");
  BLEDevice::init("");
  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);
  BLEScanResults *foundDevices = pBLEScan->start(5);
  int count = foundDevices->getCount();
  if (count == 0)
    Serial.println("No Bluetooth devices found");
  else
    Serial.printf("Found %d Bluetooth device(s)\n", count);
  pBLEScan->clearResults();
  pBLEScan->stop();
  BLEDevice::deinit(true);
  vTaskDelay(pdMS_TO_TICKS(100));
  Serial.println("/**********Bluetooth Scan Over**********/\n");
  BLE_NUM = count;
  vTaskDelete(NULL);
}

void Wifi_Scan(void)
{
  xTaskCreatePinnedToCore(
    WifiScanTask,
    "WifiScanTask",
    4096,
    NULL,
    1,
    NULL,
    0
  );
}

void Bluetooth_Scan(void)
{
  xTaskCreatePinnedToCore(
    BluetoothScanTask,
    "BluetoothScanTask",
    4096,
    NULL,
    1,
    NULL,
    0
  );
}
