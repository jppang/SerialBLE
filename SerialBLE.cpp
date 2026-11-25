
#include <NimBLEDevice.h>
#include "SerialBLE.h"
/*
// BLE Service UUID
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"  // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E" //  RX
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E" //  TX
*/
#define SERVICE_UUID        	"BLUE"
#define CHARACTERISTIC_UUID 	"1234"

NimBLECharacteristic *pCharacteristic = nullptr;
static NimBLEServer *pServer = nullptr;
bool deviceConnected = false;
bool messageReceived = false;
String rxMessage = "";

// BLE Callback functions
class MyServerCallbacks: public NimBLEServerCallbacks {
	void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) {
    	deviceConnected = true;
    	Serial.printf("Device Connected: %s\n", connInfo.toString().c_str());
		pServer->updateConnParams(connInfo.getConnHandle(), 24, 48, 0, 180);
    }
    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) {
    	deviceConnected = false;
    	Serial.println("Device Disconnected - start advertising again");
		NimBLEDevice::startAdvertising();
	}
};

// Callback functions
class MyCallbacks: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) {
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

	void onRead(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) {
        Serial.println("Client requested a BLE Read.");
    }
};


void bleSetup(String deviceName) {
	Serial.println("BLE Name: " + deviceName);  

	// Initialize BLE
	if(NimBLEDevice::init(deviceName.c_str())) Serial.println("BLE init successfully!"); // BLE's name
	pServer = NimBLEDevice::createServer();
	pServer->setCallbacks(new MyServerCallbacks());

	// Create BLE services
	NimBLEService *pService = pServer->createService(SERVICE_UUID);

	pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::READ   |
        NIMBLE_PROPERTY::WRITE  |
        NIMBLE_PROPERTY::NOTIFY
    );

	pCharacteristic->setCallbacks(new MyCallbacks());

	// Start Services
	pService->start();
	pCharacteristic->setValue("Hi There");

	// Start Advertising
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
	pAdvertising->setName(deviceName.c_str());
	if(pAdvertising->start())
		Serial.println("BLE is advertising!");
}

bool bleConnected() {
	return deviceConnected;
}

void bleWrite(String message) {
	if (deviceConnected) {
    	pCharacteristic->setValue(message.c_str());
    	pCharacteristic->notify();
    	Serial.println("Sent: " + message);
    }
}

void bleWrite(byte* message, size_t count) {
	if (deviceConnected) {
    	pCharacteristic->setValue(message, count);
		pCharacteristic->notify();
    }
}

String bleRead() {
	if(messageReceived) {
    	messageReceived = false;
    	return rxMessage;
    } else {
    	return "";
    }
}   
