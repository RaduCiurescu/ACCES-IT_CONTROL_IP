#include<Arduino.h>
#include <ESP32Servo.h>

#define SERVO 17
 // initialize the stepper library on pins 8 through 11:
 Servo myServo;
 
 void setup() {
   // set the speed at 60 rpm:
   
  myServo.attach(SERVO);
   // initialize the serial port:
   Serial.begin(9600);
 }
 
 void loop() {
   // step one revolution  in one direction:
   myServo.write(90);
   delay(2000);
   myServo.write(-90);
   delay(2000);
 }
 
 