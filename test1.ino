#define BLYNK_TEMPLATE_ID "TMPL6CrW6gMG6"
#define BLYNK_TEMPLATE_NAME "khalifa Testing"
#define BLYNK_AUTH_TOKEN "6UEbfWzQKAJUMLFtyyX4Cwaqjtjx8JvA"
//Here we are enabling output to Serial monitor of what Blynk is doing for debugging purposes.
#define BLYNK_PRINT Serial
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include "HX711.h"

//These lines below are specific to your hardware. For example, it you are working with NodeMCU, these lines will be different.
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

//wifi credentials
char ssid[] = "khalifa";
char pass[] = "xxxxxxx";

BlynkTimer timer; // Creating a timer object

Servo myservo;

bool motion = false;
int motionSensorPin = D1;
int ledPin = LED_BUILTIN;
// Variables for RFID
constexpr uint8_t RST_PIN = D3;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D4;      // Configurable, see typical pin layout above
MFRC522 rfid(SS_PIN, RST_PIN);      // Instance of the class
MFRC522::MIFARE_Key key;
// Variables for timing
unsigned long previousMillis = 0; 
const long interval = 1000; // Interval at which to check for motion (milliseconds)
const int LOADCELL_DOUT_PIN = 12;
const int LOADCELL_SCK_PIN = 13;

void scale_setup() {
  Serial.begin(115200);
  Serial.println("Initializing the scale");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  

  scale.set_scale(83);
  scale.tare();
}

void motionSetup() {
    Serial.begin(115200);
    pinMode(motionSensorPin, INPUT);
    pinMode(ledPin, OUTPUT);
}

void setupRFID() {
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Setup a timer function to be called every second
  timer.setInterval(1000L, myTimerEvent);
  motionSetup();
  setupRFID();
  scale_setup();


}

// motionSensorData = digitalRead(D1)

void loop() {
  Blynk.run();
  timer.run();
  bool motionBool;
  string camelID;
  float weightValue;

  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= interval) {
        // Save the last time you checked for motion
        previousMillis = currentMillis;
        checkForMotion(motionBool);   
  }

   if (motionBool == true)
  {
      Blynk.virtualWrite(V1, 1);
      readRFID(camelID);
      Blynk.virtualWrite(V2, CamelID);
      motor_dispense(camelID);
  }
  else
  {
      Blynk.virtualWrite(V1, 0);
  }
    
  while(!CamelFinishedEating())
  {
    weightValue = readScaleValue();
  }

  Blynk.virtualWrite(V2, weightValue);

  float consumption = 240 - weightValue;
  


  notifications(camelID, weightValue, consumptionTime);

  //reset all values for next camel.
  // find a way to store array or associated individual data.
  // find a way to retrieve amount of food.


    

}



void readRFID(&tag2) {
  if (!rfid.PICC_IsNewCardPresent()) 
    return;
  
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
    
    Serial.println(tag);
    tag2 = tag;
    tag = "";
    
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

// Declare function to check for motion
void checkForMotion(&motion) {
    int motionSensorValue = digitalRead(motionSensorPin);
    if (motionSensorValue == HIGH) {
        digitalWrite(ledPin, HIGH);
        Serial.println("Motion detected!");
        motion = true;
        return true;
    } else {
        digitalWrite(ledPin, LOW);
        Serial.println("No motion detected.");
    }
}


void motor_dispense(tag)
{
  if (tag == "19217919" || tag == "2293818595" || tag == "3393818595") {
      Serial.println("Access Granted!");
      digitalWrite(D8, HIGH);    // Turn on output on pin D8
      // Move servo for 10 seconds
      myservo.write(180); // Rotate servo to desired position (adjust as needed)
      delay(1000);      // Wait for 10 seconds
       myservo.write(0);  // Rotate servo to stop position
      delay(100);       // Small delay

    } else {
      Serial.println("Access Denied!");
      digitalWrite(D8, HIGH);   // Turn on output on pin D8
      delay(2000);
    }
}

void readScaleValues() {
  float weightValue;
  //Serial.print("one reading:\t");
  //Serial.print(scale.get_units(), 1);
  //Serial.print("\t| average:\t");
  //Serial.println(scale.get_units(10), 5);

  //Blynk.virtualWrite(V2, scale.get_units(10));
  weightValue = scale.get_units(10);
  scale.power_down();
  delay(5000);
  scale.power_up();
  return weightValue;
}

// Function to check if the camel has finished eating
bool camelFinishedEating() {
  // Read motion sensor
  int motionValue = digitalRead(motionSensorPin);

  // Read weight sensor
  int currentWeight = analogRead(weightSensorPin);

  // Wait for a brief moment to stabilize readings (optional)
  delay(100);

  // Read weight sensor again after a brief delay
  int newWeight = analogRead(weightSensorPin);

  // Calculate weight change
  int weightChange = abs(newWeight - currentWeight);

  // Check if there is no motion and no significant change in weight
  if (motionValue == LOW && weightChange < weightChangeThreshold) {
    return true;  // Camel has finished eating
  } else {
    return false;  // Camel is still eating
  }
}

void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V4, millis() / 1000);
}

void notifications(){
  //get alerts to start reloading the food and when its empty and when camel has finished
}





