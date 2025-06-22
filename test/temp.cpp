#include<Arduino.h>
#include "DHT.h"
#include <WiFi.h>
// Setează pinul de date și tipul senzorului
#define DHTPIN 26        // GPIO 4 (poți schimba)
#define DHTTYPE DHT22   // Pentru DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("Pornire senzor DHT22...");
  dht.begin();
}

void loop() {
  // Citim umiditatea și temperatura
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Celsius

  // Verificăm dacă citirea a reușit
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Eroare la citirea de la senzorul DHT22!");
    delay(2000);
  }

  // Afișăm valorile
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.print(" °C | Umiditate: ");
  Serial.print(humidity);
  Serial.println(" %");

  delay(2000); // Așteptăm 2 secunde între citiri
}