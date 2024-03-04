//function header to be included in main.c
// Created: 04/03/2024

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Arduino.h>
#include <stdio.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <string>



//fetch data from the database
//should Get the ID from RFID and pass it as a parameter to the function
//should return the type of food, ammount of food to be dispensed, and the time of the day
void fetch_data()
{
    //FN to fetch data from the server
    //return ID, current Time, type1, type2, amount1, amount2
}
//function to detect if camel is present
//is a boolean function
bool motion_detect(int sensor, int led, bool& state)
{
    bool val = digitalRead(sensor);  
    if (val == HIGH) 
    {
        digitalWrite(led, HIGH);
        delay(100);
        if (state == LOW) 
        {
            state = HIGH;
        }
        return true;
    } 
    else
    {
        digitalWrite(led, LOW);
        delay(200);
        if (state == HIGH)
        {
            state = LOW;
        }
        return false;
    }
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












#endif



// #include <LiquidCrystal.h>  // Include the LiquidCrystal library for LCD display

// // Initialize the LCD display with the pins it's connected to
// LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

// // Define the pins for the load cell module 
// #define DT A0  
// #define SCK A1  
// #define sw 2

// // Global variables for holding sensor reading and calculation
// long sample=0;  
// float val=0;  
// long count=0;

// // Function to read data from the load cell module
// unsigned long readCount(void)  
// {  
//  unsigned long Count;
//  unsigned char i;  

//  // Set up the digital pins for communication with the load cell
//  pinMode(DT, OUTPUT);  
//  digitalWrite(DT,HIGH);  
//  digitalWrite(SCK,LOW); 

//  // Initialize the Count variable
//  Count=0; 

//  // Read data from the load cell
//  pinMode(DT, INPUT);
//  while(digitalRead(DT));

//  for (i=0;i<24;i++)  
//  {
//   digitalWrite(SCK,HIGH);  
//   Count=Count<<1;  
//   digitalWrite(SCK,LOW);  
//   if(digitalRead(DT)) Count++;  
//  } 

//  // Process and return the final count value from the load cell 
//  digitalWrite(SCK,HIGH);  
//  Count=Count^0x800000;  
//  digitalWrite(SCK,LOW);  
//  return(Count);  
// }

// // The setup function runs once when you press reset or power the board
// void setup()  
// {
//  Serial.begin(9600);  // Initialize serial communication

//  // Set up the pins for load cell and switch
//  pinMode(SCK, OUTPUT);  
//  pinMode(sw, INPUT_PULLUP); 

//  // Set up the LCD screen
//  lcd.begin(16, 2);  
//  lcd.print("    Weight ");  
//  lcd.setCursor(0,1);  
//  lcd.print(" Measurement ");  

//  delay(1000);  
//  lcd.clear();  

//  // Calibrate the scale
//  calibrate();  
// }

// // The loop function runs over and over again forever
// void loop()  
// {
//  count= readCount();  // Read the load cell

//  // Calculate weight based on readings and calibration value
//  int w=(((count-sample)/val)-2*((count-sample)/val));  

//  // Print the weight to the serial monitor for debug
//  Serial.print("weight:");  
//  Serial.print((int)w);  
//  Serial.println("g");

//  // Display the weight on the LCD screen
//  lcd.setCursor(0,0);  
//  lcd.print("Weight            ");  
//  lcd.setCursor(0,1);  
//  lcd.print(w);  
//  lcd.print("g             ");

//  // If the switch is pressed, re-calibrate the scale
//  if(digitalRead(sw)==0)
//  {
//    val=0;  
//    sample=0;  
//    w=0;  
//    count=0;  
//    calibrate();  
//  }
// }

// // Function to calibrate the scale
// void calibrate()  
// {
//  lcd.clear();
//  lcd.print("Calibrating...");
//  lcd.setCursor(0,1);  
//  lcd.print("Please Wait...");

//  // Average over 100 samples for more accurate initial reading
//  for(int i=0;i<100;i++)  
//  {  
//    count=readCount();  
//    sample+=count;  
//    Serial.println(count);  
//  }  
//  sample/=100;
//  Serial.print("Avg:");  
//  Serial.println(sample);  

//  // Prompt for placing 100g weight for calibration
//  lcd.clear();  
//  lcd.print("Put 100g & wait");

//  // Loop until a satisfactory weight reading
//  count=0;  
//  while(count<1000)  
//  {
//    count=readCount();  
//    count=sample-count;  
//    Serial.println(count);
//  }

//  lcd.clear();  
//  lcd.print("Please Wait....");  
//  delay(2000);

//  // Process the 100g weight readings
//  for(int i=0;i<100;i++)
//  {
//    count=readCount();
//    val+=sample-count;
//    Serial.println(sample-count);
//  }

//  val=val/100.0;  
//  val=val/100.0; // calibrating weight is 100g  

//  lcd.clear();  
// }
