#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <WiFi.h>
#include <HTTPClient.h>
#define SERVICE_UUID        "12345678-1234-5678-1234-56789abcdef0"
#define CHARACTERISTIC_UUID "abcdef12-3456-7890-abcd-ef1234567890"
const char* ssid = "bofadn2";       // Your Wi-Fi SSID
const char* password = "12345678"; // Your Wi-Fi password

const char* serverName = "http://192.168.137.173:3000/api/send-string"; // IP address of your Express server

String code="";


void sendStringToServer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Specify the request destination
    http.begin(serverName);

    // Set headers
    http.addHeader("Content-Type", "application/json");

    // Create the JSON payload
    String payload = "{\"string\":\"" + code + "\"}";

    // Send the HTTP POST request with the string payload
    int httpResponseCode = http.POST(payload);

    // Check for response code
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Server Response: " + response);
    } else {
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Error: " + http.errorToString(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}


class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      Serial.print("Received over BLE: ");
      code=String(value.c_str());
      Serial.println(code);
      sendStringToServer();
    }
};

void setup() {
  Serial.begin(115200);
  delay(5000); // Dă timp Serial Monitor-ului să se conecteze
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  
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
    delay(2000);
}