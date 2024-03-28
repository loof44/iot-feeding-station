#include <Arduino.h>

const int kMotionSensorPin = D1;
const int kLedPin = LED_BUILTIN;

unsigned long kDebounceDelayMillis = 1000;
unsigned long kMotionCheckIntervalMillis = 150;

void setup() {
    Serial.begin(115200);

    pinMode(kLedPin, OUTPUT);
    digitalWrite(kLedPin, HIGH); // initialize with no motion detected

    pinMode(kMotionSensorPin, INPUT);

    Serial.println("Motion sensor test.");
}

void loop() {
    static unsigned long lastTriggerTimeMillis = 0;
    static unsigned long previousMotionCheckTimeMillis = 0;

    unsigned long currentMillis = millis();

    if (currentMillis - previousMotionCheckTimeMillis >= kMotionCheckIntervalMillis) {

        boolean isMotionDetected = digitalRead(kMotionSensorPin) == HIGH; // Change to boolean type

        if (isMotionDetected && currentMillis - lastTriggerTimeMillis > kDebounceDelayMillis) {

            Serial.println("Motion detected!");
            digitalWrite(kLedPin, LOW);

            lastTriggerTimeMillis = currentMillis; 
        }
        else if (!isMotionDetected) { 

            Serial.println("Motion ended!");
            digitalWrite(kLedPin, HIGH);
        }

        previousMotionCheckTimeMillis = currentMillis;
    }
}
