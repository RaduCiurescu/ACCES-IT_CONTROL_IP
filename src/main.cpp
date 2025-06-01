#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include "BluetoothSerial.h"
#define RED 26
#define GREEN 25
#define BLUE 33
#define YELLOW 32
#define SERVO 17
#define intrare_SENSOR 15  
#define iesire_SENSOR 4  
#define SERVICE_UUID        "12345678-1234-5678-1234-56789abcdef0"
#define CHARACTERISTIC_UUID "abcdef12-3456-7890-abcd-ef1234567890"

BluetoothSerial SerialBT;


enum STATE{
  IN,
  OUT,
  NONE
};

const char* ssid = "bofadn2";       // Your Wi-Fi SSID
const char* password = "12345678"; // Your Wi-Fi password

unsigned long lastPrintTime = 0;
bool wasConnected = false;

bool acces=false;
STATE currentState=NONE;
const char* serverName = "http://192.168.137.173:3000/api/send-string"; // IP address of your Express server
Servo myServo;
String code="";
bool permission=false;

String directieToString(STATE d) {
  if(d == IN) 
  return "IN" ;
  else if(d == OUT)
  return "OUT";
  else
  return "NONE";
}
void handleServerResponse(const String& response) {
  StaticJsonDocument<200> doc;

  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.print("JSON deserialization failed: ");
    Serial.println(error.c_str());
    return;
  }

  if (doc.containsKey("access")) {
    permission = doc["access"];
    Serial.print(directieToString(currentState)+"<  Permission is: ");
    Serial.println(permission ? "true" : "false");
    if(permission) {
      myServo.write(-90);
    } 
  } else {
    Serial.println("Key 'access' not found in response");
  }
}


void sendStringToServer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Specify the request destination
    http.begin(serverName);

    // Set headers
    http.addHeader("Content-Type", "application/json");

    // Create the JSON payload
    String payload = "{\"id\":\"" + code + "\",\"direction\":\""+directieToString(currentState)+"\"}";

    // Send the HTTP POST request with the string payload
    int httpResponseCode = http.POST(payload);

    // Check for response code
    if (httpResponseCode > 0) {
      String response = http.getString();
      handleServerResponse(response);
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
   pinMode(RED,OUTPUT);
   pinMode(BLUE,OUTPUT);
   pinMode(GREEN,OUTPUT);
   pinMode(YELLOW,OUTPUT);
  pinMode(intrare_SENSOR, INPUT_PULLUP);
  pinMode(iesire_SENSOR, INPUT_PULLUP);
  myServo.attach(SERVO);

   WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  
  Serial.println("BLE server started, waiting for connections...");
  SerialBT.begin("ESP32_ACCES");
  // BLEDevice::init("ESP32_GATE");
  // BLEServer *pServer = BLEDevice::createServer();
  // BLEService *pService = pServer->createService(SERVICE_UUID);
  // BLECharacteristic *pCharacteristic = pService->createCharacteristic(
  //                                        CHARACTERISTIC_UUID,
  //                                        BLECharacteristic::PROPERTY_WRITE
  //                                      );
  // pCharacteristic->setCallbacks(new MyCallbacks());
  
  // pService->start();
  // BLEDevice::startAdvertising();// Poți lăsa gol sau poți pune debug aici
  
}
/*
intra masina intre senzori---->>state in/out-->if pt state in||if pt state out
scaneaza codul de pe telefon --->> permisiuni.

*/
void loop() {
  int iesireState = digitalRead(iesire_SENSOR);
  int intrareState = digitalRead(intrare_SENSOR);
 
  if (currentState == NONE) {
    if (iesireState == LOW) {
      currentState = OUT;
      Serial.println("Detectat la iesire → OUT");
    } else if (intrareState == LOW) {
      currentState = IN;
      Serial.println("Detectat la intrare → IN");
    }
  }
 if (SerialBT.available()) {
   { code = SerialBT.readStringUntil('\n');
    Serial.println("Received: " + code);
    sendStringToServer();
    SerialBT.println("ACCES_GRANTED");
  //treat server response
  }

    // Aici acționezi bariera
  }
  // Dacă nu mai e nimic în dreptul senzorilor, resetăm
  if (currentState != NONE && iesireState == HIGH && intrareState == HIGH) {
    Serial.println("Nimic în dreptul senzorilor, revenim la NONE");
    currentState = NONE;
    code = "";
    permission = false;
    myServo.write(90); // Închide bariera
  }
  // digitalWrite(RED, HIGH); // Power up the LED
  // digitalWrite(BLUE, HIGH); // Power up the LED
  // digitalWrite(GREEN, HIGH); // Power up the LED
  // digitalWrite(YELLOW, HIGH); // Power up the LED
  // delay(1000); // this speeds up the simulation
  
  // digitalWrite(RED, LOW); // Power up the LED
  // digitalWrite(GREEN, LOW); // Power up the LED
  // digitalWrite(BLUE, LOW); // Power up the LED
  // digitalWrite(YELLOW, LOW); // Power up the LED
  // delay(1000); // this speeds
  
  delay(50);
}

  




