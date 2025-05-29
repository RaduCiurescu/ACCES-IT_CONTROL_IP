#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#define SERVICE_UUID        "12345678-1234-5678-1234-56789abcdef0"
#define CHARACTERISTIC_UUID "abcdef12-3456-7890-abcd-ef1234567890"

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      Serial.print("Received over BLE: ");
      Serial.println(value.c_str());
    }
};

void setup() {
  Serial.begin(115200);
  delay(5000); // Dă timp Serial Monitor-ului să se conecteze
  Serial.println("BLE server started, waiting for connections...");

  BLEDevice::init("ESP32_GATE");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristic->setCallbacks(new MyCallbacks());
  
  pService->start();
  BLEDevice::startAdvertising();// Poți lăsa gol sau poți pune debug aici
}

void loop() {
}