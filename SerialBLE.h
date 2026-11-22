
#ifndef SERIAL_BLE_H
#define SERIAL_BLE_H 
#include <Arduino.h>


class SerialBLE{
    public:
        SerialBLE();
        void setup(String deviceName);
        void write(String message);
        void write(byte* message, size_t count);
        String read();

    protected:
        
};

#endif
