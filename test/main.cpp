#include <Arduino.h>
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
#define intrare_SENSOR 2  
#define iesire_SENSOR 18  // GPIO8 înlocuit cu GPIO18

BluetoothSerial SerialBT;

enum STATE {
  IN,
  OUT,
  NONE
};

const char* ssid = "bofadn2";
const char* password = "12345678";
const char* serverName = "http://192.168.137.173:3000/api/send-string";
bool acces = false;
STATE currentState = NONE;
Servo myServo;
String code = "";
bool permission = false;

String directieToString(STATE d) {
  if (d == IN) return "IN";
  if (d == OUT) return "OUT";
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
    Serial.print(directieToString(currentState) + "<  Permission is: ");
    Serial.println(permission ? "true" : "false");

    if (permission) {
      myServo.write(0);  // Deschide bariera
      SerialBT.println("ACCES_GRANTED");
      delay(2000);       // Bariera rămâne deschisă 2 secunde
      myServo.write(90); // Închide bariera
    } else {
      SerialBT.println("ACCES_DENIED");
    }
  } else {
    Serial.println("Key 'access' not found in response");
  }
}

void sendStringToServer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.setTimeout(3000);  // Timeout conexiune
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"id\":\"" + code + "\",\"direction\":\"" + directieToString(currentState) + "\"}";

    int httpResponseCode = http.POST(payload);

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

void setup() {
  Serial.begin(115200);
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(intrare_SENSOR, INPUT_PULLUP);
  pinMode(iesire_SENSOR, INPUT_PULLUP);
  myServo.attach(SERVO);
  myServo.write(90); // Închide bariera inițial

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  SerialBT.begin("ESP32_ACCES");
  Serial.println("Bluetooth Serial started");
}

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
    code = SerialBT.readStringUntil('\n');
    yield();  // Previne watchdog reset
    Serial.println("Received: " + code);
    sendStringToServer();
  }

  if (currentState != NONE && iesireState == HIGH && intrareState == HIGH) {
    Serial.println("Nimic în dreptul senzorilor, revenim la NONE");
    currentState = NONE;
    code = "";
    permission = false;
    myServo.write(90); // Închide bariera
  }

  delay(100); // rulare lină
}
