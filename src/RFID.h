#ifndef RFID_h
#define RFID_h
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>


constexpr uint8_t RST_PIN = D3;     
constexpr uint8_t SS_PIN = D4;     


MFRC522 rfid(SS_PIN, RST_PIN); 
MFRC522::MIFARE_Key key;


String tag;
void setupRFID(bool &state) {
  //SPI.begin();
  rfid.PCD_Init();
  state = true;
}

 void RFID(String &tag, bool &state) {
  tag = ""; // Clear the 'tag' string ready for next card.
  
  // Check for new cards
  if (!rfid.PICC_IsNewCardPresent()){
    state = false;
    return;
  }

  Serial.println("RFID is reading..............");
 
  // If a new card is detected, read its UID
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < rfid.uid.size; i++) { // For each byte in the UID...
      tag += String(rfid.uid.uidByte[i], HEX); // ...append it to the string 'tag'.
    }

    // Print the card's UID to the serial monitor
    Serial.println(tag);
    state = true;

    // Terminate the reading process
    rfid.PICC_HaltA();

    // Stop encryption on PCD
    rfid.PCD_StopCrypto1();
  } else {
    state = false;
  }
}

#endif 