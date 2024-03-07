#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// Variables for motion sensor and LED
int motionSensorPin = D1;
int ledPin = LED_BUILTIN;

// Variables for RFID
constexpr uint8_t RST_PIN = D3;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D4;      // Configurable, see typical pin layout above
MFRC522 rfid(SS_PIN, RST_PIN);      // Instance of the class
MFRC522::MIFARE_Key key;
String tag;

// Variables for timing
unsigned long previousMillis = 0; 
const long interval = 1000; // Interval at which to check for motion (milliseconds)

// Declare setup function
void sensorSetup() {
    Serial.begin(9600);
    pinMode(motionSensorPin, INPUT);
    pinMode(ledPin, OUTPUT);
}

// Declare function to check for motion
void checkForMotion() {
    int motionSensorValue = digitalRead(motionSensorPin);
    if (motionSensorValue == HIGH) {
        digitalWrite(ledPin, HIGH);
        Serial.println("Motion detected!");
    } else {
        digitalWrite(ledPin, LOW);
        Serial.println("No motion detected.");
    }
}

void setupRFID() {
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
}

void readRFID() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
    
    Serial.println(tag);
    tag = "";
    
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

// Setup function runs once when the device starts
void setup() {
    sensorSetup();
    setupRFID();
}

void loop() {
    unsigned long currentMillis = millis();

    if(currentMillis - previousMillis >= interval) {
        // Save the last time you checked for motion
        previousMillis = currentMillis;
        checkForMotion();   
    }

    readRFID();
}




