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


void motionSensor(bool &state) {

    // Read the value from the motion sensor: HIGH when motion is detected, otherwise LOW.
    int motionSensorValue = digitalRead(motionSensorPin);

    if (motionSensorValue == HIGH) { 

        

        // Print message to console
        Serial.println("Motion detected!"); 
        state = true;
    }
    else {

       

        // Print message to console
        Serial.println("No motion detected.");
        state = false;
    }

    // Wait for a second before checking again
    delay(1000); 
}

String RFID() {
    tag = "";
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
    Serial.println(tag);
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return tag;
  }
}

void getTime() {
  const char* ssid = "loof";
  const char* password = "123456789";

  connectToNetwork(ssid, password);
    
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

  int currentMinute = timeClient.getMinutes();
  Serial.print("Minutes: ");
  Serial.println(currentMinute); 
   
  int currentSecond = timeClient.getSeconds();
  Serial.print("Seconds: ");
  Serial.println(currentSecond);  

  String weekDay = weekDays[timeClient.getDay()];
  Serial.print("Week Day: ");
  Serial.println(weekDay);    

  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int monthDay = ptm->tm_mday;
  Serial.print("Month day: ");
  Serial.println(monthDay);

  int currentMonth = ptm->tm_mon+1;
  Serial.print("Month: ");
  Serial.println(currentMonth);

  String currentMonthName = months[currentMonth-1];
  Serial.print("Month name: ");
  Serial.println(currentMonthName);

  int currentYear = ptm->tm_year+1900;
  Serial.print("Year: ");
  Serial.println(currentYear);

  //Print complete date:
  String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  Serial.print("Current date: ");
  Serial.println(currentDate);

  Serial.println("");

  delay(2000);
}

void motor(int compartment){
  if (compartment != 0){
    for (int i = 0; i < compartment; i++){
      digitalWrite(D2, HIGH);
      myservo.write(180);
      delay(1000); 
      myservo.write(0);  // Rotate servo to stop position
      delay(100);
    }
  }   
}

float readScale() {
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.println(scale.get_units(10), 5);
  float weight = scale.get_units();

  scale.power_down();             // put the ADC in sleep mode
  delay(5000);
  scale.power_up();
  return (weight);
}

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



/*****************************************************************/

// Don't forget to include this library at the top of your file
#include <ctime>

// Struct for storing session info
struct session_info {
  String camelID;
  int entryId;
  float foodDropped;
  int consumptionDuration;
  std::time_t dateTime; // This field will store the date and time
};

session_info current_session; // a global object of type `session_info`

void loop() {
  // Check for user movement
  motionSensor(motionDetected);

  if (motionDetected) {
    // Read RFID tag
    String rfidTag = RFID();
    
    // If tag is empty, return
    if (rfidTag.isEmpty()) {
      return;
    }

    // Get current time
    getTime();
    current_session.dateTime = std::time(nullptr); // Store the current date and time

    int currentHour = timeClient.getHours();

    // Determine number of rotations based on time (AM / PM)
    int rotations;
    if (currentHour < 12) {
      rotations = motorCountAM;
    } else {
      rotations = motorCountPM;
    }

    // Save data in the struct
    current_session.camelID = rfidTag;
    current_session.entryId = /* You need to decide how to set this value */;
    current_session.foodDropped = rotations;

    // Start recording consumption duration
    unsigned long startMillis = millis();

    motor(rotations);
    current_session.consumptionDuration = readScale();

    // End recording consumption duration
    unsigned long endMillis = millis();
    current_session.consumptionDuration = static_cast<int>((endMillis - startMillis) / 1000); // Time in seconds

    send_data(current_session);

    // Reset state for next run
    motionDetected = false;
  } 

  // Add a delay or a non-blocking alternative to prevent excessive loop iteration
  delay(200);
}


/*********************************************/

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <BlynkSimpleEsp8266.h>
// Don't forget to include the library

char auth[] = "YourAuthToken";  // Your Blynk Auth Token

void send_data(session_info current_session) {
  Blynk.virtualWrite(V1, current_session.camelID);
  
  Blynk.virtualWrite(V2, current_session.entryId);
  
  Blynk.virtualWrite(V3, current_session.foodDropped);
  
  Blynk.virtualWrite(V4, current_session.consumptionDuration);

  // Convert time_t to String before sending
  char buf[20];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&current_session.dateTime));
  Blynk.virtualWrite(V5, buf);
}

void setup() {
  // Initialize Blynk
  Blynk.begin(auth, ssid, pass);
  
  // Other setup code...
}

void loop() {
  // Other loop code...

  send_data(current_session);
  
  // Other loop code...

  Blynk.run();
}

/*********************************************/
void send_data_to_mqtt(session_info current_session) {
  String payload;
  StaticJsonDocument<JSON_DOC_SIZE> doc;

  // Add data to JSON document
  doc["camelID"] = current_session.camelID;
  doc["entryId"] = current_session.entryId;
  doc["foodDropped"] = current_session.foodDropped;
  doc["consumptionDuration"] = current_session.consumptionDuration;

  // Convert time_t to String before sending
  char buf[20];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&current_session.dateTime));
  doc["dateTime"] = buf;

  // Serialize JSON document to String
  serializeJson(doc, payload);

  // TODO: Replace with your MQTT publish function
  mqtt_publish(payload);
}


/*******************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "your_SSID";
const char* password =  "your_PASSWORD";
const char* mqttServer = "your_MQTT_BROKER";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
    Serial.begin(115200);
    
    setup_wifi();
    client.setServer(mqttServer, 1883);
}

void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    randomSeed(micros());

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void mqtt_publish(String payload) {
  if (!client.connected()) {
    reconnect();
  }
  
  // TODO: Replace with your topic
  client.publish("your_topic", (char*) payload.c_str());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect, just a client ID
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait for 5 seconds before retrying
      delay(5000);
    }
  }
}
/******************************************/

