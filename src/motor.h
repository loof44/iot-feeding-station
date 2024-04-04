#ifndef MOTOR_H
#define MOTOR_H
#include <Arduino.h>
#include <Servo.h>

Servo myservo;  // create servo object to control a servo

void setupMotor(){
  myservo.attach(D2);  // attaches the servo on pin D2 to the servo object
}

void motor(int compartment){
  if (compartment != 0){
    for (int i = 0; i < compartment; i++){
      digitalWrite(D2, HIGH);
      myservo.write(180);
      delay(1000); 
      myservo.write(0);  // Rotate servo to stop position
      delay(100);
    }
  }   
}

#endif