/*
A thin wrapper around the ESP32's I2C bus. I2C (Inter-Integrated Circuit) is a communication protocol that lets a microcontroller talk to multiple chips/sensors using just two wires:

SDA (data line) — carries the actual data
SCL (clock line) — keeps everything in sync

It provides:

I2C_Init() — initializes the I2C bus with the board's specific pins (SDA=15, SCL=7)
I2C_Read() and I2C_Write() — generic functions to read/write to any I2C device by address

The TCA9554PWR GPIO expander chip communicates over I2C. Without it, it can't talk to the GPIO expander, and without the GPIO expander it can't control the backlight.
*/

#pragma once
#include <Wire.h> 

// GPIO 7 to the SCL (Serial Clock Line) line of the I2C bus.
// This wire carries a regular pulse that acts like a metronome, keeping both sides in sync. 
// Every time the clock pulses, one bit of data is read from the SDA line. Without this both sides wouldn't know when to read or write data.
#define I2C_SCL_PIN       7

// GPIO 15 to the SDA (Serial Data Line) line of the I2C bus
// This is the wire that carries the actual data being sent back and forth.
#define I2C_SDA_PIN       15


void I2C_Init(void);
bool I2C_Read(uint8_t Driver_addr, uint8_t Reg_addr, uint8_t *Reg_data, uint32_t Length);
bool I2C_Write(uint8_t Driver_addr, uint8_t Reg_addr, const uint8_t *Reg_data, uint32_t Length);