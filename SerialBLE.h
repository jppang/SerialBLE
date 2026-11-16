#ifndef SERIAL_BLE_H
#define SERIAL_BLE_H 
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "config.h"

void initSerialBLE(String deviceName);
void serialBLETX(String message);
void serialBLETX(byte* message, size_t count);
String serialBLERX();
#endif