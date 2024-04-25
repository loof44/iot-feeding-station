/*************************************************************

  American University In Dubai
  Project: IoT Based Smart Camel Feeder
  Group Members: Khalifa AlSaadi, Nareeman Behiry, Alia Al Darweesh, Maryam Bin Hashem
  Senior Design Project: Iot-Enabled-Feeding-Station
 *************************************************************/

#define BLYNK_TEMPLATE_ID "TMPL6CrW6gMG6"
#define BLYNK_TEMPLATE_NAME "Feeding Station"
#define BLYNK_AUTH_TOKEN "6UEbfWzQKAJUMLFtyyX4Cwaqjtjx8JvA"

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


//char ssid[] = "loof";
//char pass[] = "123456789";
char ssid[] = "nemo";
char pass[] = "nemo1234";
//mqtt broaker
// const char* mqtt_server = "8008fde5131742c59e6a658482d0693a.s1.eu.hivemq.cloud";
// const char* mqtt_username = "username";
// const char* mqtt_password = "password";
// const int mqtt_port =8883;


// WiFiClient espClient;
// PubSubClient client(espClient);

// unsigned long lastMsg = 0;
// #define MSG_BUFFER_SIZE (50)
// char msg[MSG_BUFFER_SIZE];

// /**** Method for Publishing MQTT Messages **********/
// void publishMessage(const char* topic, String payload , boolean retained){
//   if (client.publish(topic, payload.c_str(), true))
//       Serial.println("Message publised ["+String(topic)+"]: "+payload);
// }

/************* Connect to MQTT Broker ***********/
// void reconnect() {
//   // Loop until we're reconnected
//   while (!client.connected()) {
//     Serial.print("Attempting MQTT connection...");
//     String clientId = "MQTT_FX_Client";   // Create a random client ID
//     clientId += String(random(0xffff), HEX);
//     // Attempt to connect
//     if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
//       Serial.println("connected");

//       client.subscribe("data");   // subscribe the topics here

//     } else {
//       Serial.print("failed, rc=");
//       Serial.print(client.state());
//       Serial.println(" try again in 5 seconds");   // Wait 5 seconds before retrying
//       delay(5000);
//     }
//   }
// }

bool camelFinishedEating() {
  float weightChangeThreshold = 100; // change in weight threshold
  // int time_in_seconds = millis()/1000;
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


struct BlynkData1 {
  String camelUID;
  String time;
  String foodType;
  float weightOfConsumedFood;
  float amountDropped;
  int consumptionTime;
};
void sendToBlynk(BlynkData1 data) {
  Blynk.virtualWrite(RFID_SENSOR_PIN, data.camelUID);
  delay(100);
  Blynk.virtualWrite(V5, data.time);
  delay(100);
  Blynk.virtualWrite(V6, data.weightOfConsumedFood);
  delay(100);
  Blynk.virtualWrite(V7, data.consumptionTime);
  delay(100);

  Blynk.virtualWrite(V8, data.foodType);
  delay(100);
  Blynk.virtualWrite(V9, data.amountDropped);
  delay(100);
};

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
//String time;
String Date;
String Time;
float weightOfConsumedFood;
int consumptionTime;
int startTime;

struct components {
  bool motionSensor = false;
  bool RFID = false;
  bool scale = false;
  bool network = false;
  bool time = false;
  bool motor = false;
};

struct time_log {
  int hour;
  int minute;
  int second;
  int day;
  int month;
  int year;
};


// void sendData(BlynkData1 data) {
//   DynamicJsonDocument doc(1024);


//   doc["camelUID"] = data.camelUID;
//   doc["time"] = data.time;
//   doc["weightOfConsumedFood"] = data.weightOfConsumedFood;
//   doc["consumptionTime"] = data.consumptionTime;
//   doc["foodType"] = data.foodType;
//   doc["amountDropped"] = data.amountDropped;

//   char jsonBuffer[128];
//   serializeJson(doc, jsonBuffer);  // Convert JSON document to string

//   publishMessage("data", jsonBuffer, true);
// }

time_log currentTime;
components componentStatus;

void setup(){
    Serial.begin(115200);
    Serial.println("Setting up components...");
    Serial.println("Connecting to network...");
    Serial.println("connecting to blynk...");
    
    connectToNetwork(ssid, pass, componentStatus.network);
    //Serial.println("connecting to blynk...");
    
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    delay(1000);
    //Serial.println("Connecting to MQTT Broker");
    // client.setServer(mqtt_server, mqtt_port);
    delay(1000);
    //Serial.println("setting up scale...");
    scaleSetup(componentStatus.scale);
    delay(1000);
    //Serial.println("connecting to blynk...");
    

    //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    delay(1000);
    Serial.println("setting up motion sensor...");
    motionSetup(componentStatus.motionSensor);
    delay(1000);
    Serial.println("setting up RFID...");
    SPI.begin();
    setupRFID(componentStatus.RFID);
    delay(1000);
    // scaleSetup(componentStatus.scale);
    Serial.println("setting up motor...");
    setupMotor(componentStatus.motor);
    delay(1000);
    // Serial.println("setting up network...");
    // connectToNetwork(ssid, pass, componentStatus.network);
    Serial.println("setting up time...");
    timeSetup2(componentStatus.time);
    delay(1000);
    Serial.println("Components setup complete, checking status... ");
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
      Blynk.logEvent("settup_complete");
    }
    else {
        //print the component that is not ready
        if (!componentStatus.motionSensor) {
          Serial.println("Motion sensor not ready");
          Blynk.logEvent("motion_sensor_down");
        }
        if (!componentStatus.RFID) {
          Serial.println("RFID not ready");
          Blynk.logEvent("rfid_down");

        }
        if (!componentStatus.scale) {
          Serial.println("Scale not ready");
          Blynk.logEvent("scale_down");
        }
        if (!componentStatus.network) {
          Serial.println("Network not ready");
          
        }
        if (!componentStatus.time) {
          Serial.println("Time not ready");
          Blynk.logEvent("time_down");
        }
        if (!componentStatus.motor) {
          Serial.println("Motor not ready");
          Blynk.logEvent("motor_down");
        }

      isDispenserReady = false;
    }
    
}
float currentWeight;
int total_chambers = 9;
int counter_chamber = 1;
void loop()
{   
  
  String foodType;
    float foodDropped;
    Blynk.run();
    timer.run();
    //check if chamber has been refilled
    //BLYNK_WRITE(V1);
    if (isDispenserReady){
      
      Blynk.logEvent("motion_started");
        while(!isMotionDetected){
            motionSensor(isMotionDetected);
  
        }
        if (isMotionDetected){
          Blynk.logEvent("motion_detected");
          while(!isCamelIDDetected){
            // Serial.print("Reading RFID");
            RFID(camelUID, isCamelIDDetected);
            if (camelUID != "" || camelUID != NULL)
            {
            Serial.println("camel ID:");
            Serial.println(camelUID);
            isCamelIDDetected = true;
            // if (isCamelIDDetected){
            //     Blynk.virtualWrite(RFID_SENSOR_PIN, camelUID);
            //   }
            }
            //Blynk.logEvent("rfid_read_confirm");
          }

          Blynk.logEvent("rfid_read_confirm");
          if (isCamelIDDetected){
                Blynk.virtualWrite(RFID_SENSOR_PIN, camelUID);
              }
          delay(1000);
            getTime2(currentTime.hour, currentTime.minute, currentTime.second, currentTime.day, currentTime.month, currentTime.year);
            if (currentTime.hour != 0 && currentTime.minute != 0 && currentTime.second != 0 && currentTime.day != 0 && currentTime.month != 0 && currentTime.year != 0){
                isTimeDetected = true;
                delay(1000);
                if (currentTime.hour < 12){
                   //motor(1);
                    Blynk.virtualWrite(V10, 1);
                    isFoodDropped = true;
                    //Serial.println("Morning: Dropping 2.5KG of food for camel with ID: " + camelUID);
                    Blynk.logEvent("camel_started_eating");
                    foodType = "alfalafa";
                    foodDropped = 2500;


                }
                else if (15 >currentTime.hour && currentTime.hour >= 12){
                    //motor(2);
                    Blynk.virtualWrite(V10, 1);
                    isFoodDropped = true;
                    //Serial.println("Evening: Dropping 5KG of food for camel with ID: " + camelUID);
                    Blynk.logEvent("camel_started_eating");
                }
                else if (currentTime.hour >= 15){
                    //motor(1);
                    Blynk.virtualWrite(V10, 1);
                    isFoodDropped = true;
                    //Serial.println("Evening: Dropping 2.5KG of barley for camel with ID: " + camelUID);
                    Blynk.logEvent("camel_started_eating");
                    foodType = "barley";
                    foodDropped = 2500;
                }
                else
                {
                    isFoodDropped = false;
                    Serial.println("No food dropped");
                    
                }
                                while (camelFinishedEating() == false) {
                                    isCamelDoneEating = false;
                                    Blynk.virtualWrite(V9, foodDropped);
                                    weightOfConsumedFood = readScale();
                                    //Serial.println("Camel is still eating");
                                    //Serial.println("Current weight: ");
                                    Serial.println(weightOfConsumedFood);
                                    consumptionTime = millis()/1000;

                                }
                                isCamelDoneEating = true;
                                   if (isCamelDoneEating) {
                                        //Serial.println("Camel has finished eating");
                                        currentWeight = readScale();
                                        weightOfConsumedFood = 2500.0 - currentWeight;
                                        //Serial.println("Current weight: ");
                                        //Serial.println(currentWeight);
                                        //send data to blynk
                                        Blynk.virtualWrite(V8, foodType);
                                        Blynk.virtualWrite(V6, weightOfConsumedFood);
                                        Blynk.virtualWrite(V7, consumptionTime);
                                        String time2 = String(currentTime.hour) + ":" + String(currentTime.minute) + ":" + String(currentTime.second);
                                        //Blynk.virtualWrite(V5, time2 );
                                        
                                        BlynkData1 data;
                                        data.camelUID = camelUID;
                                        data.time = String(currentTime.hour) + ":" + String(currentTime.minute) + ":" + String(currentTime.second);
                                        data.weightOfConsumedFood = weightOfConsumedFood;
                                        data.consumptionTime = millis()/1000;
                                        data.foodType = foodType;
                                        data.amountDropped = foodDropped;
                                        //sendToBlynk(data);
                                        //Serial.println("Data sent to Blynk");
                                        Blynk.logEvent("camel_stopped_eating");
                                        //sendData(data);
                                    }
                              }
                              else
                              {
                                    isTimeDetected = false;
                                    //Serial.println("No time detected");
                                    Blynk.logEvent("time_down");
                              }
                            }
                    else
                    {
                      isCamelIDDetected = false;
                     // Serial.println("No camel ID detected");
                    }
                 //check if all states are true
                    if (isDispenserReady && isMotionDetected && isCamelIDDetected && isTimeDetected && isFoodDropped && isCamelDoneEating){
                    //Serial.println("All states are true... Session Done");
                    total_chambers--;
                    counter_chamber++;
                    Blynk.virtualWrite(V4, total_chambers);
                    Blynk.virtualWrite(V10, 0);
                    
                    //reset all states
                    isMotionDetected = false;
                    isCamelIDDetected = false;
                    isTimeDetected = false;
                    isFoodDropped = false;
                    isCamelDoneEating = false;
                    }
                    else
                    {
                      //Serial.println("Session not done or error occured");
                    }

                    if (total_chambers == 0){
                    isDispenserReady = false;
                    //Serial.println("All chambers are empty");
                    //send notification to refill the compartments
                    Blynk.logEvent("chamber_empty");
                    }
                    else{
                      //Serial.println("Chambers remaining: ");
                      Serial.println(total_chambers); 
                    }
                    

                    //make sure all flags are true then send data to blynk
                    //create function to send data to blynk
                    //send alerts if any error occured mid session and have exception handling.
                  }
                  
                }

// bool camelFinishedEating() {
//   float weightChangeThreshold = 0.1; // change in weight threshold
//   int time_in_seconds = millis()/1000;
//   // Read motion sensor
//   int motionValue = digitalRead(motionSensorPin);

//   // Read weight sensor
//   float currentWeight = readScale();

//   // Wait for a brief moment to stabilize readings (optional)
//   delay(100);

//   // Read weight sensor again after a brief delay
//   int newWeight = readScale();

//   // Calculate weight change
//   float weightChange = abs(newWeight - currentWeight);

//   // Check if there is no motion and no significant change in weight
//   if (motionValue == LOW && weightChange < weightChangeThreshold) {
    
//     return true;  // Camel has finished eating
//   }
//   else
//   {
//     return false;  // Camel is still eating
//     //write the struct to send data.
//   }
// }


BLYNK_WRITE(V4)
{
  int pinValue = param.asInt();
  if (pinValue > 0)
  {
    total_chambers = pinValue;
    isDispenserReady = true;
  }
}


// struct BlynkData1 {
//   String camelUID;
//   String time;
//   String foodType;
//   float weightOfConsumedFood;
//   float amountDropped;
//   int consumptionTime;
// };
//replace the virtual pins with the actual pins
// void sendToBlynk(BlynkData1 data) {
//   Blynk.virtualWrite(RFID_SENSOR_PIN, data.camelUID);
//   Blynk.virtualWrite(V5, data.time);
//   Blynk.virtualWrite(V6, data.weightOfConsumedFood);
//   Blynk.virtualWrite(V7, data.consumptionTime);
//   Blynk.virtualWrite(V8, data.foodType);
//   Blynk.virtualWrite(V9, data.amountDropped);
// };

//server


// void sendData(BlynkData data) {
//   JsonDocument doc(1024);

//   doc["camelUID"] = data.camelUID;
//   doc["time"] = data.time;
//   doc["weightOfConsumedFood"] = data.weightOfConsumedFood;
//   doc["consumptionTime"] = data.consumptionTime;
//   doc["foodType"] = data.foodType;
//   doc["amountDropped"] = data.amountDropped;

//   char jsonBuffer[128];
//   serializeJson(doc, jsonBuffer);  // Convert JSON document to string

//   client.publish("data", jsonBuffer);
// }

