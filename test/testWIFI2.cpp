#include <WiFi.h>
#include <HTTPClient.h>
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

const char* ssid = "bofadn2";       // Your Wi-Fi SSID
const char* password = "12345678"; // Your Wi-Fi password

const char* serverName = "http://192.168.137.173:3000/api/send-string"; // IP address of your Express server

void sendStringToServer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Specify the request destination
    http.begin(serverName);

    // Set headers
    http.addHeader("Content-Type", "application/json");

    // Create the JSON payload
    String payload = "{\"string\":\"bofa\"}";

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

void setup() {
  Serial.begin(115200);
 //SerialBT.begin("ESP32_Bariera");
 //Serial.print("Bluetooth MAC address: ");
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Send the string "bofa" to the server
  sendStringToServer();
}

void loop() {

  sendStringToServer();
  delay(2000);
}