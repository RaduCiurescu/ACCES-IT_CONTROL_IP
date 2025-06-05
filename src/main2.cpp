#include "BluetoothSerial.h"
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include "FS.h"
#include "SPIFFS.h"

#define RED 18
#define GREEN 17
#define BLUE 19
#define YELLOW 16
#define SERVO 15
#define intrare_SENSOR 4 
#define iesire_SENSOR 15  


BluetoothSerial SerialBT;

enum STATE {
  IN,
  OUT,
  NONE
};

const char* ssid = "bofadn2";
const char* password = "12345678";
const char* serverName1 = "http://192.168.5.156:3000/api/free-access";
const char* serverName2 = "http://192.168.5.156:3000/api/verify-access-from-mobile";
const char* serverName3 = "http://192.168.5.156:3000/api/manual-open";
unsigned long lastPrintTime = 0;
bool wasConnected = false;
bool acces = false;
STATE currentState = NONE;
Servo myServo;
String code = "";
bool permission = false;
bool internetSTATE=false;


void openGate()
{
  Serial.println("DESCHIDE BARIERA");
  myServo.write(90); // Deschide bariera
  digitalWrite(BLUE,HIGH);
  digitalWrite(GREEN,HIGH);
  delay(1000);
  digitalWrite(GREEN,LOW);
  delay(2000);
  myServo.write(0); // Închide bariera
  digitalWrite(BLUE,LOW);
}
void rejectResponse()
{
      Serial.println("REJECCCCCCCCCCCCCCCCTTTTTTTT");
      digitalWrite(RED,HIGH);
      delay(1000);
      digitalWrite(RED,LOW);
    
}
void checkServerForFreeAccess() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName1); // Update this endpoint if needed
    http.setTimeout(2000);
    
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Free Access Server Response: " + response);

      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, response);

      if (!error && doc.containsKey("access")) {
        if(doc["access"] == true){
        openGate();
      }
      else if(doc["access"]==2)
      {
        rejectResponse();
      }}
    
    } else {
      Serial.println("Free access check failed. Code: " + String(httpResponseCode));
    }

    http.end();
  }
}

void checkServerForFreeAccess2() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName3); // Update this endpoint if needed
    http.setTimeout(1000);
    
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("OPEN MANUAL Server Response: " + response);

      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, response);

      if (!error && doc.containsKey("access") && doc["access"] == true) {
        openGate();
      }
    } else {
      Serial.println("OPEN MANUAL check failed. Code: " + String(httpResponseCode));
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


int verificaAcces(String codCautat) {
  File file = SPIFFS.open("/config.csv", "r");
  if (!file) {
    Serial.println("Eroare la deschiderea fisierului CSV");
    return -1;
  }

  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim(); // elimina spatii si \r

    int fieldIndex = 0;
    int start = 0;
    String codGasit = "";
    String acces = "";

    for (int i = 0; i <= line.length(); i++) {
      if (line[i] == ',' || i == line.length()) {
        String field = line.substring(start, i);
        field.trim();

        if (fieldIndex == 4) codGasit = field;       // Coloana 5
        if (fieldIndex == 5) acces = field;          // Coloana 6

        fieldIndex++;
        start = i + 1;
      }
    }

    if (codGasit == codCautat) {
      file.close();
      if (acces == "true") return 1;
      else return 0;
    }
  }

  file.close();
  return -1; // codul nu a fost gasit
}


void setup() {
  Serial.begin(115200);
 
//   if (!SPIFFS.begin(true)) {
//   Serial.println("Eroare la montarea SPIFFS");
// }

  pinMode(intrare_SENSOR, INPUT_PULLUP);
  pinMode(iesire_SENSOR, INPUT_PULLUP);
  myServo.attach(SERVO);
  myServo.write(90); // Închide bariera inițial

 
  pinMode(RED,OUTPUT);
  pinMode(GREEN,OUTPUT);
  pinMode(BLUE,OUTPUT);
  pinMode(YELLOW,OUTPUT);
 
  SerialBT.begin("ESP32_ACCESS"); // Numele ESP32 ca dispozitiv Bluetooth
  WiFi.begin(ssid, password);
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
  }
  else
  Serial.println("not connected to WiFi");

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
    digitalWrite(YELLOW,LOW);
  }
  else{
    digitalWrite(YELLOW,HIGH);
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

  if(iesireState==LOW || intrareState==LOW)  //current state
  {
  // Citire date de la client Bluetooth
  if (SerialBT.available())
   {  code = SerialBT.readStringUntil('\n');
    if (code == "REJECT")
    {
      rejectResponse();
    }
      if( code=="TRUE")
      {
        openGate(); //pieton
      }
      else{
      sendStringToServer(); //vehicul
      SerialBT.println("ACCES_GRANTED");
      }
  }
  else
    {
      checkServerForFreeAccess();
      //checkServerForFreeAccess2();
    }
    Serial.println("senzorii detecteaza");
  }
  else{
    Serial.println("senzorii nu detecteaza nimic");
  }

    if (currentState != NONE && iesireState == HIGH && intrareState == HIGH) 
    {
    Serial.println("Nimic în dreptul senzorilor, revenim la NONE");
    currentState = NONE;
    code = "";
    permission = false;
    myServo.write(90); // Închide bariera
  }
  delay(1000);
}
