#include <Arduino.h>
void setup() {
  Serial.begin(9600);
}

void loop() {
  int valoare = analogRead(A1);        // Citim senzorul LM35 conectat la pinul A1
  float temperatura = valoare * 0.488; // Convertim direct în °C (5V și 10mV/°C)

  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" °C");

  delay(1000); // Pauză de 1 secundă
}
