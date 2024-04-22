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


char ssid[] = "SSID";
char pass[] = "PASSWORD";
//mqtt broaker
const char* mqtt_server = "8008fde5131742c59e6a658482d0693a.s1.eu.hivemq.cloud";
const char* mqtt_username = "khalifa";
const char* mqtt_password = "Khalifa1003";
const int mqtt_port =8883;


WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

/**** Method for Publishing MQTT Messages **********/
void publishMessage(const char* topic, String payload , boolean retained){
  if (client.publish(topic, payload.c_str(), true))
      Serial.println("Message publised ["+String(topic)+"]: "+payload);
}

/************* Connect to MQTT Broker ***********/
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";   // Create a random client ID
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");

      client.subscribe("led_state");   // subscribe the topics here

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");   // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}





BlynkTimer timer;

//Define states


bool isTimeDetected = false;
bool isDispenserReady = false;
bool isMotionDetected = false;
bool isCamelIDDetected = false;
bool isFoodDropped = false;
bool isCamelDoneEating = false;

//Define datastreams
String camelUID;
String time;
String Date;
String Time;
float weightOfConsumedFood;
int consumptionTime;
int startTime;

struct components {
  bool motionSensor;
  bool RFID;
  bool scale;
  bool network;
  bool time;
  bool motor;
};

struct time_log {
  int hour;
  int minute;
  int second;
  int day;
  int month;
  int year;
};
time_log currentTime;
components componentStatus;

void setup(){
    Serial.begin(115200);
    client.setServer(mqtt_server, mqtt_port);
    

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    motionSetup(componentStatus.motionSensor);
    setupRFID(componentStatus.RFID);
    scaleSetup(componentStatus.scale);
    setupMotor(componentStatus.motor);
    connectToNetwork(ssid, pass, componentStatus.network);
    timeSetup(componentStatus.time);
    for (int i = 0; i < 6; i++) {
      Serial.println(componentStatus.motionSensor);
      Serial.println(componentStatus.RFID);
      Serial.println(componentStatus.scale);
      Serial.println(componentStatus.network);
      Serial.println(componentStatus.time);
      Serial.println(componentStatus.motor);

    }
    //check if all components are ready
    if (componentStatus.motionSensor && componentStatus.RFID && componentStatus.scale && componentStatus.network && componentStatus.time && componentStatus.motor) {
      isDispenserReady = true;
    }
    else {
        //print the component that is not ready
        if (!componentStatus.motionSensor) {
          Serial.println("Motion sensor not ready");
        }
        if (!componentStatus.RFID) {
          Serial.println("RFID not ready");
        }
        if (!componentStatus.scale) {
          Serial.println("Scale not ready");
        }
        if (!componentStatus.network) {
          Serial.println("Network not ready");
        }
        if (!componentStatus.time) {
          Serial.println("Time not ready");
        }
        if (!componentStatus.motor) {
          Serial.println("Motor not ready");
        }

      isDispenserReady = false;
    }
    
}

void loop()
{
    String foodType;
    float foodDropped;
    int total_chambers = 6;//configure the number of chambers
    Blynk.run();
    timer.run();
    //check if chamber has been refilled
    //BLYNK_WRITE(V1);
    if (isDispenserReady){
        while(!isMotionDetected){
            motionSensor(isMotionDetected);
        }
        if (isMotionDetected){
            camelUID = RFID();
            if (camelUID != "" || camelUID != NULL)
            {
            Serial.println("camel ID:");
            Serial.println(camelUID);
            isCamelIDDetected = true;
            getTime2(currentTime.hour, currentTime.minute, currentTime.second, currentTime.day, currentTime.month, currentTime.year);
            if (currentTime.hour != 0 && currentTime.minute != 0 && currentTime.second != 0 && currentTime.day != 0 && currentTime.month != 0 && currentTime.year != 0){
                isTimeDetected = true;
                if (currentTime.hour < 12){
                    motor(1);
                    isFoodDropped = true;
                    Serial.println("Morning: Dropping 2.5KG of food for camel with ID: " + camelUID);
                    foodType = "alfalafa";
                    foodDropped = 2.5;


                }
                else if (15 >currentTime.hour >= 12){
                    motor(2);
                    isFoodDropped = true;
                    Serial.println("Evening: Dropping 5KG of food for camel with ID: " + camelUID);
                }
                else if (currentTime.hour >= 15){
                    motor(1);
                    isFoodDropped = true;
                    Serial.println("Evening: Dropping 2.5KG of barley for camel with ID: " + camelUID);
                    foodType = "barley";
                    foodDropped = 2.5;
                }
                else
                {
                    isFoodDropped = false;
                    Serial.println("No food dropped");
                    Blynk.logEvent("No food dropped");
                }
                while (camelFinishedEating() == false) {
                    isCamelDoneEating = false;
                    weightOfConsumedFood = readScale();
                    Serial.println("Camel is still eating");
                    Serial.println("Current weight: ");
                    Serial.println(weightOfConsumedFood);
                    consumptionTime = millis()/1000;
                }
                isCamelDoneEating = true;
                   if (isCamelDoneEating) {
                        Serial.println("Camel has finished eating");
                        weightOfConsumedFood = readScale();
                        Serial.println("Current weight: ");
                        Serial.println(weightOfConsumedFood);
                        //send data to blynk
                        BlynkData data;
                        data.camelUID = camelUID;
                        data.time = String(currentTime.hour) + ":" + String(currentTime.minute) + ":" + String(currentTime.second);
                        data.weightOfConsumedFood = weightOfConsumedFood;
                        data.consumptionTime = millis()/1000 - startTime;
                        data.foodType = foodType;
                        data.amountDropped = foodDropped;
                        sendToBlynk(data);
                        Serial.println("Data sent to Blynk");
                        Blynk.logEvent("Camel with ID: " + camelUID + " has finished eating");
                        sendData(data);
                    }
              }
              else
              {
                    isTimeDetected = false;
                    Serial.println("No time detected");
                    Blynk.logEvent("No time detected");
              }
            }
    else
    {
      isCamelIDDetected = false;
      Serial.println("No camel ID detected");
    }
 //check if all states are true
    if (isDispenserReady && isMotionDetected && isCamelIDDetected && isTimeDetected && isFoodDropped && isCamelDoneEating){
    Serial.println("All states are true... Session Done");
    total_chambers--;
    //reset all states
    isMotionDetected = false;
    isCamelIDDetected = false;
    isTimeDetected = false;
    isFoodDropped = false;
    isCamelDoneEating = false;
    }
    else
    {
      Serial.println("Session not done or error occured");
    }

    if (total_chambers == 0){
    isDispenserReady = false;
    Serial.println("All chambers are empty");
    //send notification to refill the compartments
    Blynk.logEvent("All compartments are empty. Please refill the compartments");
    }
    else{
      Serial.println("Chambers remaining: ");
      Serial.println(total_chambers); 
    }
    

    //make sure all flags are true then send data to blynk
    //create function to send data to blynk
    //send alerts if any error occured mid session and have exception handling.
    }
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
  }
  else
  {
    return false;  // Camel is still eating
    //write the struct to send data.
  }
}

//Complete the code in which assiging values a virtual pin in blynk to update of the compartments used.
//if 0 then then compartments empty then send notification to application to change the status of the compartment to empty.
//notify people to refill the compartments.
BLYNK_WRITE(V1)
{
  int pinValue = param.asInt();
  if (pinValue == 6)
  {
    isDispenserReady = true;
  }
}

struct BlynkData {
  String camelUID;
  String time;
  String foodType;
  float weightOfConsumedFood;
  float amountDropped;
  int consumptionTime;
};
//replace the virtual pins with the actual pins
void sendToBlynk(BlynkData data) {
  Blynk.virtualWrite(V1, data.camelUID);
  Blynk.virtualWrite(V2, data.time);
  Blynk.virtualWrite(V3, data.weightOfConsumedFood);
  Blynk.virtualWrite(V4, data.consumptionTime);
  Blynk.virtualWrite(V5, data.foodType);
  Blynk.virtualWrite(V6, data.amountDropped);
};

//server


void sendData(BlynkData data) {
  DynamicJsonDocument doc(1024);

  doc["camelUID"] = data.camelUID;
  doc["time"] = data.time;
  doc["weightOfConsumedFood"] = data.weightOfConsumedFood;
  doc["consumptionTime"] = data.consumptionTime;
  doc["foodType"] = data.foodType;
  doc["amountDropped"] = data.amountDropped;

  char jsonBuffer[128];
  serializeJson(doc, jsonBuffer);  // Convert JSON document to string

  client.publish("data", jsonBuffer);
}

