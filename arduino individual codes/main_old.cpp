/*************************************************************

  American University In Dubai
  Project: IoT Based Smart Camel Feeder
  Group Members: Khalifa AlSaadi, Nareeman Behiry, Alia Al Darweesh, Maryam Bin Hashem
  Senior Design Project: Iot-Enabled-Feeding-Station
 *************************************************************/


#define BLYNK_TEMPLATE_ID           "TMPL6CrW6gMG6"
#define BLYNK_TEMPLATE_NAME         "khalifa Testing"
#define BLYNK_AUTH_TOKEN            "6UEbfWzQKAJUMLFtyyX4Cwaqjtjx8JvA"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SPI.h>
#include <MFRC522.h>
#include "HX711.h"
#include <Servo.h>
#include <ArduinoJson.h>
#include <RFID.h>



// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 12;
const int LOADCELL_SCK_PIN = 13;

// Declare and Initialize motion sensor pin to D1
int motionSensorPin = D1; 

// Set Reset pin and slave pin for RFID
constexpr uint8_t RST_PIN = D3;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D4;     // Configurable, see typical pin layout above

// Create an instance of the class servo named myservo
Servo myservo;

// Create an instance of the class HX711 named scale
HX711 scale;

// Create an instance of the class MFRC522 named rfid with the SS_PIN and RST_PIN
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

// Create an instance of the class MFRC522::MIFARE_Key named key
MFRC522::MIFARE_Key key;


// Create an instance of the class BlynkTimer named timer
// BlynkTimer is a blynk class that allows you to start, stop, and run timers.
BlynkTimer timer;

// create a variable to store the RFID tag
String tag;


// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Khalifa";
char pass[] = "ENTER_PASSWORD_HERE";

//setup function runs once when you press reset or power the board

void setup()
{
  // Debug console
  Serial.begin(115200);
  // Set the motion sensor pin as an Input pin
  pinMode(motionSensorPin, INPUT);
  
  /* Blynk.begin is a function that initializes the Blynk library and establishes a connection to the Blynk server
  It takes the BLYNK_AUTH_TOKEN, ssid, and pass as arguments.

  You can also specify server:
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);
  */

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);


  //Initializes SPI Bus and MFRC522 for RFID functionality
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522

  // Initialize load cell and set scale
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(83);
  scale.tare();
  delay(100);
}

void loop()
{
  // Call the Blynk.run() function to establish a connection to the Blynk server
  Blynk.run();
  // Call the timer.run() function to run the timer
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
  timer.setInterval(5000L, motion);
}

/************************************************************************************************/
// void RFID() {
//   if ( ! rfid.PICC_IsNewCardPresent())
//     return;
//   if (rfid.PICC_ReadCardSerial()) {
//     for (byte i = 0; i < 4; i++) {
//       tag += rfid.uid.uidByte[i];
//     }
//     Serial.println(tag);
//     Blynk.virtualWrite(V1, tag);
//     tag = "";
//     rfid.PICC_HaltA();
//     rfid.PCD_StopCrypto1();
//   }
// }
/************************************************************************************************/
void motion() {

    // Read the value from the motion sensor: HIGH when motion is detected, otherwise LOW.
    int motionSensorValue = digitalRead(motionSensorPin);

    if (motionSensorValue == HIGH) { 

        // If motion is detected, turn on the LED
        

        // Print message to console
        Serial.println("Motion detected!");
        Blynk.virtualWrite(V0, 1);
    }
    else {

        // If no motion is detected, turn off the LED
        

        // Print message to console
        Serial.println("No motion detected.");
        Blynk.virtualWrite(V0, 0);
    }

    // Wait for a second before checking again
    delay(5000);
}
/************************************************************************************************/
void scale_fn() {
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.println(scale.get_units(10), 5);
  Blynk.virtualWrite(scale.get_units());

  scale.power_down();             // put the ADC in sleep mode
  delay(5000);
  scale.power_up();
}
/************************************************************************************************/
struct session_info {
  int entryId;
  String camelID;
  float Consumption;
  int foodDropped;
  int consumptionDuration;
  
};

/************************************************************************************************/

void send_data(struct session_info session){
  int eID = session.entryId;
  String camelID = session.camelID;
  float Consumotion = session.Consumption;
  int foodDropped = session.foodDropped;
  int consumptionDuration = session.consumptionDuration;

  
   
  DynamicJsonDocument doc(1024);
  

  doc["deviceId"] = "NodeMCU";
  doc["Entry"] = eID;
  doc["camel ID "] = camelID;
  doc["Consumed Amount"] = Consumotion;
  doc["Food Dropped"] = foodDropped;
  doc["Consumption Duration"] = consumptionDuration;

  char mqtt_message[128];
  serializeJson(doc, mqtt_message);

  //publishMessage("esp8266_data", mqtt_message, true);

  delay(5000);

}
/************************************************************************************************/
// Fix the saveData function
// void saveData(struct session_info session, int duration){
//   int e_id = 0;
//   session.entryId = e_id;
//   e_id++;
//   session.camelID = BLYNK_WRITE(V1);
//   session.Consumption = BLYNK_WRITE(V2);
//   session.foodDropped = BLYNK_WRITE(V3);
//   session.consumptionDuration = duration;
// }

/************************************************************************************************/


void motor(){
  int pinV3;
  BLYNK_WRITE(pinV3);
  if (pinV3 != 0){
    for (int i = 0; i < pinV3; i++){
      digitalWrite(D2, HIGH);
      myservo.write(180);
      delay(1000); 
      myservo.write(0);  // Rotate servo to stop position
      delay(100);
    }
  }   
}

/************************************************************************************************/

BLYNK_WRITE(V3)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("Chambers to drop: ");
  Serial.println(pinValue);
}
/************************************************************************************************/
// bool camelFinishedEating() {
//   int time_in_seconds = millis()/1000;
//   // Read motion sensor
//   int motionValue = digitalRead(motionSensorPin);

//   // Read weight sensor
//   float currentWeight = analogRead(weightSensorPin);

//   // Wait for a brief moment to stabilize readings (optional)
//   delay(100);

//   // Read weight sensor again after a brief delay
//   int newWeight = analogRead(weightSensorPin);

//   // Calculate weight change
//   float weightChange = abs(newWeight - currentWeight);

//   // Check if there is no motion and no significant change in weight
//   if (motionValue == LOW && weightChange < weightChangeThreshold) {
//     return true;  // Camel has finished eating
//   } else {
//     return false;  // Camel is still eating
//     //write the struct to send data.
//   }
// }
/************************************************************************************************/
BLYNK_WRITE(V2)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("Chambers to drop: ");
  Serial.println(pinValue);
}
/************************************************************************************************/
BLYNK_WRITE(V1)
{
  //int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("RFID Number: ");
  //Serial.println(pinValue);
  //return i;
}


