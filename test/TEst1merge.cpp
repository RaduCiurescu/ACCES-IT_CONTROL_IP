#include <Arduino.h>
#include <ESP32Servo.h>
#define LED1 26
#define LED2 25
#define LED3 33
#define LED4 32
#define SERVO 17
#define intrare_SENSOR 15  
#define iesire_SENSOR 4  


Servo myServo;


void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
   pinMode(LED1,OUTPUT);
   pinMode(LED2,OUTPUT);
   pinMode(LED3,OUTPUT);
   pinMode(LED4,OUTPUT);
  pinMode(intrare_SENSOR, INPUT_PULLUP);
  pinMode(iesire_SENSOR, INPUT_PULLUP);
  myServo.attach(SERVO);
  
}

void loop() {
  
  int iesireState = digitalRead(iesire_SENSOR);
  int intrareState = digitalRead(intrare_SENSOR);
  if (iesireState == LOW && intrareState == LOW) {
                
   digitalWrite(LED1, HIGH); // Power up the LED RED
   Serial.println("2 senzori  activi");
    myServo.write(-100);
    //senzorul detecteaza
  } else if(iesireState == HIGH && intrareState == HIGH) {
  Serial.println("2 senzori  inactivi");   //nu detecteaza
    myServo.write(100);
    digitalWrite(LED1, LOW); // Power up the LED
    digitalWrite(LED2, LOW); // Power up the LED
    digitalWrite(LED3, LOW); // Power up the LED
    digitalWrite(LED4, LOW); // Power up the LED
  }
  else if(iesireState == LOW && intrareState == HIGH) {
  Serial.println("iesire senzor activ");   //nu detecteaza 
    myServo.write(-100);
  digitalWrite(LED3, HIGH); // Power up the LED

  }
  else if(iesireState == HIGH && intrareState == LOW) { 
  Serial.println("intrare senzor activ");   //nu detecteaza
    myServo.write(-100);
  digitalWrite(LED4, HIGH); // Power up the LED
  }

  // digitalWrite(LED2, HIGH); // Power up the LED
  // digitalWrite(LED3, HIGH); // Power up the LED
  // digitalWrite(LED4, HIGH); // Power up the LED
  // delay(1000); // this speeds up the simulation
  
  // digitalWrite(LED1, LOW); // Power up the LED
  // digitalWrite(LED2, LOW); // Power up the LED
  // digitalWrite(LED3, LOW); // Power up the LED
  // digitalWrite(LED4, LOW); // Power up the LED
  // delay(1000); // this speeds

  

}



