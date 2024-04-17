#include <ESP8266WiFi.h>
#include "network.h"

void connectToNetwork(const char* ssid, const char* password, bool &state) {
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    state = true;

    Serial.println("Connected to WiFi");
}
