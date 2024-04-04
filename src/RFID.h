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
void setupRFID() {
  SPI.begin();
  rfid.PCD_Init();
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
#endif 