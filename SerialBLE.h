
#ifndef SERIAL_BLE_H
#define SERIAL_BLE_H 
#include <Arduino.h>


void bleSetup(String deviceName);
void bleWrite(String message);
void bleWrite(byte* message, size_t count);
bool bleConnected();
String bleRead();


#endif
