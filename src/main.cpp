#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SERVICE_UUID "4a0d0a22-d01e-4a1f-81da-a0e5a5e75ea5"
#define CHARACTERISTIC_UUID "bc3931ca-36fc-4cbf-9854-9eb8db9dfaad"

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic)
    {
        String value = pCharacteristic->getValue();
        if (value.length() > 0) {
            Serial.print("Received text: ");
            Serial.println(value.c_str());
        }
    }
};

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("Starting BLE Server...");
    BLEDevice::init("ESP32_BLE_Server");
    BLEServer* pServer = BLEDevice::createServer();
    BLEService* pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic* pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    pCharacteristic->setValue("Hello World");
    pCharacteristic->setCallbacks(new MyCallbacks());
    pService->start();
    BLEAdvertising* pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
    Serial.println("BLE Server started, waiting for clients...");
    Serial.println("Service UUID: " + String(SERVICE_UUID));
    Serial.println("Characteristic UUID: " + String(CHARACTERISTIC_UUID));
    Serial.println("Waiting for clients to connect...");
    Serial.println("You can now connect to the ESP32 BLE Server using a BLE client.");
}

void loop()
{
    // put your main code here, to run repeatedly:
}