#include "SerialBLE.h"

// BLE Service UUID
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"  // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E" //  RX
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E" //  TX

BLECharacteristic *pCharacteristicTX;
bool deviceConnected = false;
bool messageReceived = false;
String rxMessage = "";

// BLE Callback functions
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Device Connected");
    }
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Device Disconnected");
      // Advertising
      pServer->startAdvertising();
    }
};

// RX Callback function
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      rxMessage = pCharacteristic->getValue();
      rxMessage.trim();
      if (rxMessage.length() > 0) {
        Serial.print("Received: ");
        for (int i = 0; i < rxMessage.length(); i++) {
          Serial.print(rxMessage[i]);
        }
        Serial.println();
      }
      messageReceived = true;
    }
};

void initSerialBLE(String deviceName) {
  Serial.print("BLE Name: ");
  Serial.println(deviceName);  

  // Initialize BLE
  BLEDevice::init(deviceName.c_str()); // BT's name
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE services
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create TX characteristic
  pCharacteristicTX = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  // Create RX characteristic
  BLECharacteristic *pCharacteristicRX = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_RX,
                      BLECharacteristic::PROPERTY_WRITE
                    );
  pCharacteristicRX->setCallbacks(new MyCallbacks());

  // Start Services
  pService->start();
  pServer->getAdvertising()->start();
}

void serialBLETX(String message) {
  if (deviceConnected) {
      pCharacteristicTX->setValue(message.c_str());
      pCharacteristicTX->notify();
      Serial.println("Sent: " + message);
    }
}

void serialBLETX(byte* message, size_t count) {
  if (deviceConnected) {
      pCharacteristicTX->setValue(message, count);
      pCharacteristicTX->notify();
    }
}

String serialBLERX() {
  if(messageReceived) {
    messageReceived = false;
    return rxMessage;
    } else {
    return "";
    }
}   
