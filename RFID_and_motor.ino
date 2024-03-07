// Importing necessary libraries
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// Constant pin layout setup
constexpr uint8_t RST_PIN = D3;     // Reset pin
constexpr uint8_t SS_PIN = D4;     // Slave Select pin

// Creating an instance of MFRC522 class for RFID reader
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

// Creating a servo object to control a servo motor
Servo myservo;

String tag;      

void setup() {
  Serial.begin(9600);    // Initializing serial communication at 9600 baud rate
  SPI.begin();           // Initiate SPI bus
  rfid.PCD_Init();       // Initiate MFRC522 RFID reader
  pinMode(D8, OUTPUT);   // Set the mode of pin D8 as output
  myservo.attach(D2);    // Attach the servo on pin D2 to the servo object
}

void loop() {
  if ( ! rfid.PICC_IsNewCardPresent())  // If no new card is present
    return;
  
  // Check if we can read the card's serial number
  if (rfid.PICC_ReadCardSerial()) {

    // Read the uidBytes and append to 'tag' string
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
    Serial.println(tag);

    // If the tag matches any of the predefined tags, grant access
    if (tag == "19217919" || tag == "2293818595" || tag == "3393818595") {
      Serial.println("Access Granted!");
      digitalWrite(D8, HIGH);    // Turn on output on pin D8
      // Move servo for 10 seconds
      myservo.write(180); // Rotate servo to desired position (adjust as needed)
      delay(1000);      // Wait for 10 seconds
       myservo.write(0);  // Rotate servo to stop position
      delay(100);       // Small delay

    } else {
      Serial.println("Access Denied!");
      digitalWrite(D8, HIGH);   // Turn on output on pin D8
      delay(2000);
    }
    
    digitalWrite(D8, LOW);    // Turn off output on pin D8
    tag = "";                  // Clear the 'tag' string
    rfid.PICC_HaltA();         // Halt PICC
    rfid.PCD_StopCrypto1();    // Stop encryption on PCD
  }
}