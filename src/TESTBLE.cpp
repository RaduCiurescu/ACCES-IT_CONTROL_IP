#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

unsigned long lastPrintTime = 0;
bool wasConnected = false;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_ACCESS"); // Numele ESP32 ca dispozitiv Bluetooth
}

void loop() {
  // Verificăm dacă a trecut 1 secundă
  if (millis() - lastPrintTime >= 2000) {
    lastPrintTime = millis();
    
    if (SerialBT.hasClient()) {
      if (!wasConnected) {
        Serial.println("Device connected.");
        wasConnected = true;
      }
      
      // Nu putem obține direct numele dispozitivului conectat cu BluetoothSerial.
      // Dar putem afișa că este conectat.
      Serial.println("Status: Connected.");
    } else {
      if (wasConnected) {
        Serial.println("Device disconnected.");
        wasConnected = false;
      }
      Serial.println("Status: Not connected.");
    }
  }

  // Citire date de la client Bluetooth
  if (SerialBT.available()) {
   { String data = SerialBT.readStringUntil('\n');
    Serial.println("Received: " + data);
    SerialBT.println("ACCES_GRANTED");}

    // Aici acționezi bariera
  }
}
