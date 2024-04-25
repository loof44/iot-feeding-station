#ifndef MOTION_h
#define MOTION_h
#include <Arduino.h>

// Declare and Initialize motion sensor pin to D1
int motionSensorPin = D1; 

// Declare and Initialize built-in LED pin 
int ledPin = LED_BUILTIN; 


void motionSetup(bool &state) {

    // Begin serial communication at 9600 bits per second
   

    // Set the motion sensor pin as an Input pin
    pinMode(motionSensorPin, INPUT);

    // Set the built-in LED pin as an Output pin
    pinMode(ledPin, OUTPUT); 
    state = true;
    

}

void motionSensor(bool &state) {

    // Read the value from the motion sensor: HIGH when motion is detected, otherwise LOW.
    int motionSensorValue = digitalRead(motionSensorPin);

    if (motionSensorValue == HIGH) { 

        // If motion is detected, turn on the LED
        digitalWrite(ledPin, HIGH);
       // Blynk.virtualWrite(V11, 1);
        // Print message to console
        Serial.println("Motion detected!"); 
        state = true;
    }
    else {

        // If no motion is detected, turn off the LED
        digitalWrite(ledPin, LOW);
        //Blynk.virtualWrite(V11, 0);
        // Print message to console
        Serial.println("No motion detected.");
        state = false;
    }

    // Wait for a second before checking again
    delay(1000); 
}

#endif