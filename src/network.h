// #ifndef NETWORK_H
// #define NETWORK_H
// #include <ESP8266WiFi.h>
// #include <NTPClient.h>
// #include <WiFiUdp.h>

// // Your WiFi credentials.
// // Set password to "" for open networks.
// char ssid[] = "loof";
// char pass[] = "123456789";

// void setupNetwork(){
//     // Connect to Wi-Fi
//     Serial.print("Connecting to ");
//     Serial.println(ssid);
//     WiFi.begin(ssid, pass);
//     while (WiFi.status() != WL_CONNECTED) {
//         delay(500);
//         Serial.print(".");
//     }
// }

// #endif



#ifndef NETWORK_H
#define NETWORK_H

void connectToNetwork(const char* ssid, const char* password, bool &state);

#endif
