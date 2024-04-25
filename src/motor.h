#ifndef MOTOR_H
#define MOTOR_H
#include <Arduino.h>
#include <Servo.h>

Servo myservo;  // create servo object to control a servo


void setupMotor(bool &state){
  myservo.attach(D2);  // attaches the servo on pin D2 to the servo object
    state = true;
}

void motor(int number){
  //digitalWrite(D2, HIGH);
  //myservo.write(number);
  if (number != 0){
    for (int i = 0; i < number; i++) {
    digitalWrite(D2, HIGH);
    //myservo.attach(D2);
    myservo.write(90);
   // myservo.detach();
    }
    //digitalWrite(D2, LOW);

  
    
  }   
}

#endif