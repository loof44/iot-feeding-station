// RFID.h

#ifndef RFID_H 
#define RFID_H 

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <MFRC522.h>
#include <string>

class RFIDReader {
  public:
    // Constructor that initializes the class with specified resetPin and ssPin.
    RFIDReader(uint8_t resetPin, uint8_t ssPin): rfid(ssPin, resetPin) {}

    // Setup function to initialize RFID reader module.
    void setup() {
      SPI.begin();        // Init SPI bus
      rfid.PCD_Init();    // Init MFRC522 card
    }

    // Function to read the RFID card's data.
    String readRFID() {
      
      // Check for new cards
      if (!rfid.PICC_IsNewCardPresent()) {
        return "";
      }
      // Verify if the NUID has been readed
      if (!rfid.PICC_ReadCardSerial()) {
        return "";
      }

      // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
      MFRC522::MIFARE_Key key;
      for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

      String content= "";

      // Get UID
      for (byte i = 0; i < 4; i++) {
        content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(rfid.uid.uidByte[i], HEX));
      }

      return content;
    }

  private:
    MFRC522 rfid;
};

#endif  
