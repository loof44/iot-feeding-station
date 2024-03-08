#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include "HX711.h"
#include <Servo.h>

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

//variables for HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 12;
const int LOADCELL_SCK_PIN = 13;


HX711 scale;

// Declare setup function
void sensorSetup() {
    Serial.begin(9600);
    pinMode(motionSensorPin, INPUT);
    pinMode(ledPin, OUTPUT);
}

//scale calibration setup
void scaleSetup() {
  Serial.begin(9600);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  calibrateScale();
}

//scale calibration function
void calibrateScale() {
  if (scale.is_ready()) {
    scale.set_scale();    
    Serial.println("Tare... remove any weights from the scale.");
    delay(5000);
    scale.tare();
    Serial.println("Tare done...");
    Serial.print("Place a known weight on the scale...");
    delay(5000);
    long reading = scale.get_units(10);
    Serial.print("Result: ");
    Serial.println(reading);
  } 
  else {
    Serial.println("HX711 not found.");
  }
  delay(1000);
}

//scale reading function
void readScale() {
  if (scale.is_ready()) {
    Serial.print("one reading:\t");
    Serial.print(scale.get_units(), 1);
    Serial.print("\t| average:\t");
    Serial.println(scale.get_units(10), 5);
  } 
  else {
    Serial.println("HX711 not found.");
  }
  scale.power_down();             // put the ADC in sleep mode
  delay(5000);
  scale.power_up();
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

//declare servo motor
Servo myServo;

void setupServo() {
  myServo.attach(D2);
}

void moveServo() {
  myServo.write(90); // Rotate the servo motor to 90 degrees
  delay(1000); // Wait for a second (1000 milliseconds)
  myServo.write(0); // Rotate the servo motor back to 0 degrees
  delay(1000); // Wait for another second
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




