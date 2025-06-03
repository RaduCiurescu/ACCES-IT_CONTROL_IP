#include "BluetoothSerial.h"
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#define RED 26
#define GREEN 25
#define BLUE 33
#define YELLOW 32
#define SERVO 17
#define intrare_SENSOR 2  
#define iesire_SENSOR 15  


BluetoothSerial SerialBT;

enum STATE {
  IN,
  OUT,
  NONE
};

const char* ssid = "bofadn2";
const char* password = "12345678";
const char* serverName1 = "http://192.168.127.234:3000/api/free-access";
const char* serverName2 = "http://192.168.127.234:3000/api/verify-access-from-mobile";
unsigned long lastPrintTime = 0;
bool wasConnected = false;
bool acces = false;
STATE currentState = NONE;
Servo myServo;
String code = "";
bool permission = false;



void checkServerForFreeAccess() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName1); // Update this endpoint if needed
    http.setTimeout(3000);
    
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Free Access Server Response: " + response);

      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, response);

      if (!error && doc.containsKey("access") && doc["access"] == true) {
        Serial.println("DESCHIDE BARIERA");
        myServo.write(0); // Deschide bariera
        delay(3000);
        myServo.write(90); // Închide bariera
      }
    } else {
      Serial.println("Free access check failed. Code: " + String(httpResponseCode));
    }

    http.end();
  }
}


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
    Serial.print("<  Permission is: ");
    Serial.println(permission ? "true" : "false");

    if (permission) {
      SerialBT.println("ACCES_GRANTED");
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
    http.begin(serverName2);
    http.setTimeout(3000);  // Timeout conexiune
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"id\":\"" + code + "\"}";

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
 
  pinMode(intrare_SENSOR, INPUT_PULLUP);
  pinMode(iesire_SENSOR, INPUT_PULLUP);
  myServo.attach(SERVO);
  myServo.write(90); // Închide bariera inițial

 
 
 
  SerialBT.begin("ESP32_ACCESS"); // Numele ESP32 ca dispozitiv Bluetooth
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
  // Verificăm dacă a trecut 1 secundă
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.println("trying to connect");
    //digitalWrite(YELLOW,HIGH);
  }
  ////bt
  if (SerialBT.hasClient()) {
  if (!wasConnected) {
    Serial.println("Device connected.");
    wasConnected = true;
  }
} else {
  Serial.println("Status: Not connected.");
  wasConnected = false;
}


   if (currentState == NONE) {
    if (iesireState == LOW) {
      currentState = OUT;
      Serial.println("Detectat la iesire → OUT");
    } else if (intrareState == LOW) {
      currentState = IN;
      Serial.println("Detectat la intrare → IN");
    }
  }

  // Citire date de la client Bluetooth
  if (SerialBT.available()) {
   {  code = SerialBT.readStringUntil('\n');

    sendStringToServer();
    SerialBT.println("ACCES_GRANTED");}
    // Aici acționezi bariera
  }else
    {
      checkServerForFreeAccess();
    }


    if (currentState != NONE && iesireState == HIGH && intrareState == HIGH) {
    Serial.println("Nimic în dreptul senzorilor, revenim la NONE");
    currentState = NONE;
    code = "";
    permission = false;
    myServo.write(90); // Închide bariera
  }
  delay(1000);
}
