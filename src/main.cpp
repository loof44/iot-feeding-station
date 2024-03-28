/*************************************************************

  You’ll need:
   - Blynk IoT app (download from App Store or Google Play)
   - ESP8266 board
   - Decide how to connect to Blynk
     (USB, Ethernet, Wi-Fi, Bluetooth, ...)

  There is a bunch of great example sketches included to show you how to get
  started. Think of them as LEGO bricks  and combine them as you wish.
  For example, take the Ethernet Shield sketch and combine it with the
  Servo example, or choose a USB sketch and add a code from SendData
  example.
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
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

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 12;
const int LOADCELL_SCK_PIN = 13;
int motionSensorPin = D1; 
constexpr uint8_t RST_PIN = D3;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D4;     // Configurable, see typical pin layout above


Servo myservo;
HX711 scale;

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;



BlynkTimer timer;
String tag;


// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Khalifa";
char pass[] = "0509292290";

void setup()
{
  // Debug console
  Serial.begin(115200);
  pinMode(motionSensorPin, INPUT);


  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);
  
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(83);
  scale.tare();
  delay(100);
}

void loop()
{
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
  timer.setInterval(5000L, motion);
}

/************************************************************************************************/
void RFID() {
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
    Serial.println(tag);
    Blynk.virtualWrite(V1, tag);
    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}
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
void saveData(struct session_info session, int duration){
  int e_id = 0;
  session.entryId = e_id;
  e_id++;
  session.camelID = BLYNK_WRITE(V1);
  session.Consumption = BLYNK_WRITE(V2);
  session.foodDropped = BLYBK_WRITE(V3);
  session.consumptionDuration = duration;
}

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
