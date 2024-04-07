#ifndef MOTOR_H
#define MOTOR_H
#include <Arduino.h>
#include <Servo.h>

Servo myservo;  // create servo object to control a servo

void setupMotor(bool &state){
  myservo.attach(D2);  // attaches the servo on pin D2 to the servo object
    state = true;
}

void motor(int compartment){
  if (compartment != 0){
    for (int i = 0; i < compartment; i++){
      digitalWrite(D2, HIGH);
      myservo.write(compartment*36); //each compartment is a 36 degree rotation (i think?)
      delay(1000); 
    }
  }   
}

#endif