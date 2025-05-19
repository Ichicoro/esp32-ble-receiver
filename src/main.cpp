#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SERVICE_UUID "4a0d0a22-d01e-4a1f-81da-a0e5a5e75ea5"
#define CHARACTERISTIC_UUID "bc3931ca-36fc-4cbf-9854-9eb8db9dfaad"

// Define the I2C Pins
#define SCL_PIN 32
#define SDA_PIN 33

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C  // Common I2C address for SSD1306
#define OLED_RESET -1   // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display;

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic)
    {
        std::string stdValue = pCharacteristic->getValue();
        String value = String(stdValue.c_str());
        if (value.length() > 0) {
            Serial.print("Received text: ");
            Serial.println(value.c_str());
            
            // Update OLED display with received text
            display.clearDisplay();
            display.setCursor(0, 0);
            display.println("Received:");
            display.setCursor(0, 16);
            display.println(value);
            display.display();
        }
    }
};

void setup()
{
    // Initialize serial communication
    Serial.begin(115200);
    
    // Initialize I2C
    Wire.begin(SDA_PIN, SCL_PIN);
    display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
    delay(100);

    // Try scanning for I2C devices to verify connection
    Serial.println("Scanning I2C bus...");
    byte error, address;
    int deviceCount = 0;
    for (address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println();
            deviceCount++;
        }
    }
    if (deviceCount == 0) {
        Serial.println("No I2C devices found!");
    }

    // Initialize the OLED display
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) { 
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }
    
    // Initial display settings
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Hello World");
    display.setCursor(0, 16);
    display.println("BLE Ready");
    display.display();
    
    // Initialize BLE Server
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