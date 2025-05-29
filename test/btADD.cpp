#include "BluetoothSerial.h"

  BluetoothSerial SerialBT;
void setup() {
  Serial.begin(115200);

  // Inițializează Bluetooth
  SerialBT.begin("ESP32_Bariera");

  // Afișează MAC-ul Bluetooth în Serial Monitor
  Serial.print("Bluetooth MAC address: ");
  Serial.println(SerialBT.getBtAddressString());
}

void loop() {
  // Nu ai nevoie de nimic aici pentru a vedea MAC-ul
  Serial.println(SerialBT.getBtAddressString());
}