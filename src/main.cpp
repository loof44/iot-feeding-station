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

#include <Arduino.h>
#include <ArduinoJson.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Khalifa";
char pass[] = "ENTER_PASSWORD_HERE";

BlynkTimer timer;

//Define datastreams
String camelUID;


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
    camelUID = RFID();
    Serial.println("camel ID:");
    Serial.println(camelUID);
  }

  
}

// Determine how much food to drop
void dropFood(int time, String ID) {
  
}
