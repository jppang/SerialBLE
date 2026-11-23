
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "SerialBLE.h"
/*
// BLE Service UUID
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"  // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E" //  RX
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E" //  TX
*/
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *pCharacteristic;
bool deviceConnected;
bool messageReceived;
String rxMessage;

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

	void onRead(BLECharacteristic* pCharacteristic) override {
        Serial.println("Client requested a BLE Read.");
    }
};

SerialBLE::SerialBLE() {
	deviceConnected = false;
	messageReceived = false;
	rxMessage = "";
}

void SerialBLE::setup(String deviceName) {
	Serial.print("BLE Name: ");
	Serial.println(deviceName);  

	// Initialize BLE
	BLEDevice::init(deviceName.c_str()); // BLE's name
	BLEServer *pServer = BLEDevice::createServer();
	pServer->setCallbacks(new MyServerCallbacks());

	// Create BLE services
	BLEService *pService = pServer->createService(SERVICE_UUID);

	pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ   |
        BLECharacteristic::PROPERTY_WRITE  |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    pCharacteristic->addDescriptor(new BLE2902());

	// Start Services
	pService->start();
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // Helps with iPhone connection issues
    BLEDevice::startAdvertising();
}

bool SerialBLE::connected() {
	return deviceConnected;
}

void SerialBLE::write(String message) {
	if (deviceConnected) {
    	pCharacteristic->setValue(message.c_str());
    	pCharacteristic->notify();
    	Serial.println("Sent: " + message);
    }
}

void SerialBLE::write(byte* message, size_t count) {
	if (deviceConnected) {
    	pCharacteristic->setValue(message, count);
		pCharacteristic->notify();
    }
}

String SerialBLE::read() {
	if(messageReceived) {
    	messageReceived = false;
    	return rxMessage;
    } else {
    	return "";
    }
}   
