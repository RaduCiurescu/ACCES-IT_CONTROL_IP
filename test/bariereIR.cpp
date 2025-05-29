/*
  IR Breakbeam sensor demo with one LED!
*/
#include <Arduino.h>
#include <ESP32Servo.h>
#define RED 26
#define GREEN 25
#define BLUE 33
#define YELLOW 32
#define SERVO 17
#define intrare_SENSOR 15  
#define iesire_SENSOR 4 

// variables will change:
int sensorState = 0, lastState = 0; // variable for reading the sensor status
Servo myServo;
void setup() {
Serial.begin(115200);
   pinMode(RED,OUTPUT);
   pinMode(BLUE,OUTPUT);
   pinMode(GREEN,OUTPUT);
   pinMode(YELLOW,OUTPUT);
  pinMode(intrare_SENSOR, INPUT_PULLUP);
  pinMode(iesire_SENSOR, INPUT_PULLUP);
  myServo.attach(SERVO);

  // Start serial communication at 9600 baud rate:
  Serial.begin(9600);
}

void loop() {
  // Read the state of the IR sensor:
  sensorState = digitalRead(SENSORPIN);

  // Check if the sensor beam is broken (LOW means the beam is broken):
  if (sensorState == LOW) {
    // Turn LED on when the beam is broken:
    digitalWrite(LEDPIN, HIGH);  
  } 
  else {
    // Turn LED off when the beam is unbroken:
    digitalWrite(LEDPIN, LOW);   
  }

  // Print state change to serial monitor:
  if (sensorState == HIGH && lastState == LOW) {
    Serial.println("Unbroken");
  } 
  if (sensorState == LOW && lastState == HIGH) {
    Serial.println("Broken");
  }

  // Update last state to the current state:
  lastState = sensorState;
}
