#ifndef MOTION_h
#define MOTION_h
#include <Arduino.h>

constexpr uint8_t motionSensorPin = D1;
constexpr uint8_t ledPin = LED_BUILTIN;

inline void motionSetup(bool &state) {
    pinMode(motionSensorPin, INPUT);
    pinMode(ledPin, OUTPUT);
    state = true;
}

inline bool readMotionSensor() {
    const int motionSensorValue = digitalRead(motionSensorPin);
    digitalWrite(ledPin, motionSensorValue);
    return motionSensorValue == HIGH;
}

inline void motionSensor(bool &state) {
    state = readMotionSensor();
}

#endif
