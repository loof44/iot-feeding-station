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

#include <BlynkSimpleEsp8266.h>

#include <motion.h>
#include <RFID.h>
#include <scale.h>
#include <time.h>
#include <Arduino.h>
#include <network.h>
#include <time.h>
#include <motor.h>

#include <Arduino.h>
#include <ArduinoJson.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Khalifa";
char pass[] = "ENTER_PASSWORD_HERE";

BlynkTimer timer;

//Define datastreams
String camelUID;

struct session_info {
  int entryId;
  String camelID;
  float Consumption;
  int foodDropped;
  int consumptionDuration;
  
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  motionSetup();
  setupRFID();
  scaleSetup();
  connectToNetwork(ssid, pass);
  timeSetup();
}

void loop() {
  session_info session;
  bool motionDetected = false;
  // put your main code here, to run repeatedly:
  Blynk.run();
  // Call the timer.run() function to run the timer
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
  while (motionDetected == false) {
    motionSensor(motionDetected);
  
  }
  if (motionDetected == true) {
    
    session.camelID = camelUID;
    Serial.println("camel ID:");
    Serial.println(camelUID);
  }
  int time = getTime();
  int startTime;
  float weight;
  int consumptionTime;
  dropFood(time, camelUID);
  weight = readScale();
  while (camelFinishedEating() == false) {
    weight = readScale();
    Serial.println("Camel is still eating");
    Serial.println("Current weight: ");
    Serial.println(weight);
    consumptionTime = millis()/1000;
  }
  if (camelFinishedEating() == true) {
    Serial.println("Camel has finished eating");
    Serial.println("Current weight: ");
  }

  //put the below in a function and send upload data once the entire struct has been filled. beloow there is a function.
  session.camelID = camelUID;
  session.Consumption = weight;
  session.consumptionDuration = consumptionTime;
  session.foodDropped = 0; //food dropped
  //session.datetime = getTime();
}

// Determine how much food to drop
void dropFood(int time, String ID) {
  if (00 < time < 12){
    motor(1);
    Serial.println("Morning: Dropping 2.5KG of food for camel with ID: " + ID);
  }
  else if (12 < time < 15){
    motor(2);
    Serial.println("Afternoon: Dropping 5KG of food for camel with ID: " + ID);
  }
  else{
    motor(1);
    Serial.println("Evening: Dropping 2.5KG of barley for camel with ID: " + ID);
  }
}
bool camelFinishedEating() {
  float weightChangeThreshold = 0.1; // change in weight threshold
  int time_in_seconds = millis()/1000;
  // Read motion sensor
  int motionValue = digitalRead(motionSensorPin);

  // Read weight sensor
  float currentWeight = readScale();

  // Wait for a brief moment to stabilize readings (optional)
  delay(100);

  // Read weight sensor again after a brief delay
  int newWeight = readScale();

  // Calculate weight change
  float weightChange = abs(newWeight - currentWeight);

  // Check if there is no motion and no significant change in weight
  if (motionValue == LOW && weightChange < weightChangeThreshold) {
    
    return true;  // Camel has finished eating
  } else {
    return false;  // Camel is still eating
    //write the struct to send data.
  }
}

//fix the below function to send the struct to the server
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

// remaining is sending data to blynk server