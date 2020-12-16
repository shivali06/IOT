#include <Arduino.h>

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Adafruit_BME680.h>
#include <Wire.h>
       
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define BME_CS 23
#define ESP32_SDI 21
#define ESP32_SCK 22
#define SEALEVELPRESSURE_HPA (1013.25)

BLEServer* pServer = NULL;
//BLECharacteristic* pCharacteristic = NULL;

bool deviceConnected = true;
bool oldDeviceConnected = false;

Adafruit_BME680 bme; // I2C

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

BLECharacteristic pCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );


void setup() {
  Wire.begin(ESP32_SDI, ESP32_SCK);
  Serial.begin(9600);
  Serial.println("Starting BLE work!");
  bme.begin();
  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    while (1);
  }
  BLEDevice::init("deviceserver");
  BLEServer  *pServer =  BLEDevice::createServer();
 pServer->setCallbacks(new MyServerCallbacks());
   BLEService *pService = pServer->createService(SERVICE_UUID);
//  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
//                                         CHARACTERISTIC_UUID,
//                                         BLECharacteristic::PROPERTY_READ |
//                                         BLECharacteristic::PROPERTY_WRITE
//                                       );
pService->addCharacteristic(&pCharacteristic);
pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  bme.setTemperatureOversampling(BME680_OS_8X); 
  
}

void loop() {  
  //delay(1000);
  //Serial.println(bme.temperature);
  char txString[8];
  dtostrf(bme.readTemperature(),1,2,txString);
 pCharacteristic.setValue(txString);
 Serial.print("Temperature = ");
  Serial.print(bme.temperature);
  Serial.println(" *C");
  
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}