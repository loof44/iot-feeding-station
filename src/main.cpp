//American University In Dubai
//Project: IoT Based Smart Camel Feeder
//Group Members: Khalifa AlSaadi, Nareeman Behiry, Alia Al Darweesh, Maryam Bin Hashem
//Senior Design Project

#include <Arduino.h>
#include "HX711.h"
#include "scale.h"
#include "RFID.h"
#include "functions.h"

constexpr uint8_t RST_PIN = D3; 
constexpr uint8_t SS_PIN = D4; 

RFIDReader rfidReader(RST_PIN, SS_PIN); 

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 12;
const int LOADCELL_SCK_PIN = 13;

enum SignalStrength {
  SIGNAL_LOW = 1,
  SIGNAL_MEDIUM = 2,
  SIGNAL_HIGH = 3
};

HX711 scale;

void setup() {
  Serial.begin(115200);
  Serial.println("HX711 Demo");

  beginScale(scale, LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  displayScaleInfo(scale, "Before setting up the scale:");

  calibrateScale(scale, -478.507);
  displayScaleInfo(scale, "After setting up the scale:");

  Serial.println("Readings:");
  // Serial.begin(9600);
  rfidReader.setup();
}

int value = 0;


void loop() {
  //the below code should make the node print the weight 
  // Serial.print("one reading:\t");
  // Serial.print(scale.get_units(), 1);
  // Serial.print("\t| average:\t");
  // Serial.println(scale.get_units(10), 5);

  // scale.power_down();  
  // delay(5000);
  // scale.power_up();
  //below code is extra
  // String tag = rfidReader.readRFID();
  //   if (tag != "") {
  //       Serial.println(tag);
  //   }
  //   delay(1000);

  
  while ((motion_detect(2, 8, SignalStrength::SIGNAL_LOW) == true)) {
    
    delay(1000);
    
  }
  Serial.println("Camel is present, start reading RFID");
  String tag = rfidReader.readRFID();
    if (tag != "") {
        Serial.println(tag);
    }

    //fetch data from the database
  
  fetch_data(tag);
  int type;
  double amount;
  int current_time = 1200;
  foodType(current_time, type, amount);
  if((tag != "") && (motion_detect(2, 8, SignalStrength::SIGNAL_LOW) == true))
  {
    
    dispense(type, amount);
  }

  // weight_sensor(); 
  // calculate_consumption();




}

void printRFID() {
  String tag = rfidReader.readRFID();
  if (tag != "") {
    Serial.println(tag);
  }
  delay(1000);
}

//fetch data from the database
//should Get the ID from RFID and pass it as a parameter to the function
//should return the type of food, ammount of food to be dispensed, and the time of the day
void fetch_data(String tag)
{
    //FN to fetch data from the server
    //return ID, current Time, type1, type2, amount1, amount2
    
}
//function to detect if camel is present
//is a boolean function
bool motion_detect(int sensor, int led, enum SignalStrength)
{
  bool val = digitalRead(sensor);  

  if (val == SignalStrength::SIGNAL_LOW) 
  {
    digitalWrite(led, 3);
    delay(100);
    val = digitalRead(sensor);
    if (val == SignalStrength::SIGNAL_HIGH)
      return true;
  } 
  else
  {
    digitalWrite(led, 1);
    delay(200);
    val = digitalRead(sensor);
    if (val == SignalStrength::SIGNAL_LOW)
      return false;
  }
  return false;
}


// String readRFID()
// {
//     String tag;
//     //FN to return RFID Data Key
//     //read RFID UID
//     //return ID
//     if ( ! rfid.PICC_IsNewCardPresent())  // If no new card is present
//     return;
  
//   // Check if we can read the card's serial number
//     if (rfid.PICC_ReadCardSerial()) {

//     // Read the uidBytes and append to 'tag' string
//     for (byte i = 0; i < 4; i++) {
//       tag += rfid.uid.uidByte[i];
//     }
//     Serial.println(tag);

//     return tag;
// }
// }

void weight_sensor()
{
    
    //FN to read weight sensor
    //return weight
}

void foodType(int current_time, int &type, double &amount){
    double dispense_amount;
    if (0000 < current_time < 1200 )
    {
        dispense_amount = amount;
        //if time is between 0000 and 1200
        amount = 2.5;
        type = 1;
    }
    else if (1200 < current_time < 1800)
    {
        amount = 5.0;
        type = 1;
    }
    else if (1800 < current_time < 2400)
    {
        amount = 2.5;
        type = 2;
        
    }

    

}

void dispense(int foodType, int amount)
{
    //FN to dispense food
    //dispense food based on time
    //return foodType, dispense_amount
}


void weight_sensor()
{
    //FN to read weight sensor
    //return weight

}

void calculate_consumption()
{
    //FN to calculate consumption, duration
    //return consumption, 
}

//append data and done for now