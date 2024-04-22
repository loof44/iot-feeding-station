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
#define MOTION_SENSOR_PIN V0
#define RFID_SENSOR_PIN V1
#define SCALE_SENSOR_PIN V2
#define MOTOR_PIN V3
#define CHAMBERS_PIN V4

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Blynk.h>
#include <Arduino.h>
#include <time.h>
#include <RFID.h>
#include <motion.h>
#include <scale.h>
#include <network.h>
#include <BlynkSimpleEsp8266.h>
#include <motor.h>
#include <time2.h>
#include <PubSubClient.h>

//settup code

char ssid[] = "loof";
char pass[] = "123456789";
//mqtt broaker
const char* mqtt_server = "8008fde5131742c59e6a658482d0693a.s1.eu.hivemq.cloud";
const char* mqtt_username = "khalifa";
const char* mqtt_password = "Khalifa1003";
const int mqtt_port =8883;


void connectToNetwork(const char* ssid, const char* password, bool &state) {
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    state = true;

    Serial.println("Connected to WiFi");
}


void timeSetup2(bool &state) {

// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(14400);
  state = true;
}

void RFID_Setup(){
  
}

void getTime2(int &hour, int &minute, int &second, int &day, int &month, int &year) {
  // const char* ssid = "loof";
  // const char* password = "123456789";

  // connectToNetwork(ssid, password, state);
    
  // Your code to get time
  timeClient.update();

  time_t epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Formatted Time: ");
  Serial.println(formattedTime);  

  int currentHour = timeClient.getHours();
  Serial.print("Hour: ");
  Serial.println(currentHour);
  hour = currentHour;  

  int currentMinute = timeClient.getMinutes();
  Serial.print("Minutes: ");
  Serial.println(currentMinute); 
  minute = currentMinute;
   
  int currentSecond = timeClient.getSeconds();
  Serial.print("Seconds: ");
  Serial.println(currentSecond);  
  second = currentSecond;

  String weekDay = weekDays[timeClient.getDay()];
  Serial.print("Week Day: ");
  Serial.println(weekDay);    

  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int monthDay = ptm->tm_mday;
  Serial.print("Month day: ");
  Serial.println(monthDay);
  day = monthDay;

  int currentMonth = ptm->tm_mon+1;
  Serial.print("Month: ");
  Serial.println(currentMonth);
  month = currentMonth;

  String currentMonthName = months[currentMonth-1];
  Serial.print("Month name: ");
  Serial.println(currentMonthName);

  int currentYear = ptm->tm_year+1900;
  Serial.print("Year: ");
  Serial.println(currentYear);
  year = currentYear;

  //Print complete date:
  String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  Serial.print("Current date: ");
  Serial.println(currentDate);

  Serial.println("");
  

  delay(2000);
}

void RFID(String &tag2, bool &state) {
    // Your code to read RFID
    // state = true;
    
    state = false;
    if (!rfid.PICC_IsNewCardPresent())
    return;

  // If a new card is detected, read its UID.
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) { // For each byte in the UID...
      tag += rfid.uid.uidByte[i]; // ...append it to the string 'tag'.
    }

    // Print the card's UID to the serial monitor.
    Serial.println(tag);
    tag2 = tag;
    state = true;

    // Clear the 'tag' string ready for next card.
    tag = "";

    // Terminate the reading process.
    rfid.PICC_HaltA();

    // Stop encryption on PCD.
    rfid.PCD_StopCrypto1();
  }
}

