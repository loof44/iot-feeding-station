//Dynamic Pin Allocation inspired by Round Robin Scheduling Algorithm

#define PIN D1
#define SENSOR_CHECK_INTERVAL 1000 // Check sensors every second.

#include <Arduino.h>

enum Sensor { 
    NONE,
    MOTION,
    RFID 
};

Sensor currentSensor = NONE;
unsigned long lastCheckTime = 0;

bool isMotionSensorActive(bool &state) {
    if (state == true) {
        return true;
    }else {
        return false;
    }
}

bool isRFIDSensorActive(bool &state) {
    if (state == true) {
        return true;
    }else {
        return false;
    }
}

void updateCurrentSensor() {
    bool state1 = false;
    bool state2 = false;
    unsigned long currentTime = millis();
    
    // If enough time has passed since we last checked...
    if (currentTime - lastCheckTime >= SENSOR_CHECK_INTERVAL) {
        if (isMotionSensorActive(state1)) { // If motion sensor is active...
            currentSensor = MOTION;   // Mark the motion sensor as the current one.
        } else if (isRFIDSensorActive(state2)) { // If RFID sensor is active...
            currentSensor = RFID;     // Mark the RFID sensor as the current one.
        } else {
            currentSensor = NONE;     // No active sensor.
        }
        
        lastCheckTime = currentTime;
    }
}

void setup() {
    pinMode(PIN, INPUT);
}

void loop() {
    updateCurrentSensor(); // Check if a new sensor has become active.
    
    switch (currentSensor) {
        case MOTION:
            // Read from the motion sensor.
            break;
        case RFID:
            // Read from the RFID sensor.
            break;
        default:
            // No active sensor. Do nothing or go to sleep.
            break;
    }
}