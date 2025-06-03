#include<Arduino.h>
#define SENSOR_INTRARE 2
#define SENSOR_IESIRE 15  // Dacă dă eroare, schimbă cu 18

void setup() {
  Serial.begin(115200);

  pinMode(SENSOR_INTRARE, INPUT_PULLUP);
  pinMode(SENSOR_IESIRE, INPUT_PULLUP);

  Serial.println("Pornit - monitorizare senzori IR");
}

void loop() {
  bool intrare = digitalRead(SENSOR_INTRARE) == LOW; // LOW = fascicul întrerupt
  bool iesire  = digitalRead(SENSOR_IESIRE) == LOW;

  if (intrare) {
    Serial.println("Obstacol detectat la INTRARE");
  }

  if (iesire) {
    Serial.println("Obstacol detectat la IESIRE");
  }

  if (!intrare && !iesire) {
    Serial.println("Niciun obstacol detectat");
  }

  delay(200);
}
