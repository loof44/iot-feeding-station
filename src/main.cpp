/*************************************************************

  American University In Dubai
  Project: IoT Based Smart Camel Feeder
  Group Members: Khalifa AlSaadi, Nareeman Behiry, Alia Al Darweesh, Maryam Bin Hashem
  Senior Design Project: Iot-Enabled-Feeding-Station
 *************************************************************/

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include <PubSubClient.h>
#include <math.h>
#include <time.h>

#include <SPI.h>

#include "RFID.h"
#include "motion.h"
#include "scale.h"
#include "network.h"
#include "motor.h"
#include "time2.h"

namespace {
constexpr char WIFI_SSID[] = "nemo";
constexpr char WIFI_PASS[] = "nemo1234";

constexpr char MQTT_HOST[] = "8008fde5131742c59e6a658482d0693a.s1.eu.hivemq.cloud";
constexpr char MQTT_USERNAME[] = "username";
constexpr char MQTT_PASSWORD[] = "password";
constexpr uint16_t MQTT_PORT = 8883;

constexpr char TELEMETRY_TOPIC[] = "feeding/data";
constexpr char EVENT_TOPIC[] = "feeding/events";
constexpr char STATUS_TOPIC[] = "feeding/status/components";
constexpr char COMMAND_TOPIC[] = "feeding/commands";

constexpr unsigned long IDLE_DELAY_MS = 75;
constexpr unsigned long INACTIVITY_TIMEOUT_MS = 15000; // camel considered done after 15s inactivity
constexpr float WEIGHT_CHANGE_THRESHOLD = 100.0f;      // grams
constexpr uint8_t WEIGHT_SAMPLE_COUNT = 5;
constexpr uint16_t WEIGHT_SAMPLE_DELAY_MS = 40;
}

BearSSL::WiFiClientSecure espClient;
PubSubClient client(espClient);

struct Components {
  bool motionSensor = false;
  bool RFID = false;
  bool scale = false;
  bool network = false;
  bool time = false;
  bool motor = false;
};

struct TimeLog {
  int hour = 0;
  int minute = 0;
  int second = 0;
  int day = 0;
  int month = 0;
  int year = 0;
};

enum class FeedingState {
  Idle,
  WaitingForMotion,
  WaitingForCamel,
  Dispensing,
  Monitoring
};

struct FeedingContext {
  FeedingState state = FeedingState::Idle;
  String camelUID;
  TimeLog timestamp;
  String foodType;
  float foodDropped = 0.0f;
  float weightStart = 0.0f;
  float lastWeight = 0.0f;
  unsigned long sessionStartMs = 0;
  unsigned long lastActivityMs = 0;
};

Components componentStatus;
TimeLog currentTime;
FeedingContext ctx;

bool isDispenserReady = false;
int total_chambers = 9;

void publishJson(const char* topic, const JsonDocument& doc, bool retained = false) {
  char buffer[512];
  const size_t len = serializeJson(doc, buffer, sizeof(buffer));
  client.publish(topic, buffer, len, retained);
}

void publishEvent(const String& type, const String& detail = "") {
  StaticJsonDocument<256> doc;
  doc["event"] = type;
  if (detail.length() > 0) {
    doc["detail"] = detail;
  }
  doc["chambersRemaining"] = total_chambers;
  doc["timestamp"] = millis();
  publishJson(EVENT_TOPIC, doc);
}

void reportComponentStatus() {
  StaticJsonDocument<256> doc;
  doc["motion"] = componentStatus.motionSensor;
  doc["rfid"] = componentStatus.RFID;
  doc["scale"] = componentStatus.scale;
  doc["network"] = componentStatus.network;
  doc["time"] = componentStatus.time;
  doc["motor"] = componentStatus.motor;
  doc["ready"] = isDispenserReady;
  doc["chambers"] = total_chambers;
  publishJson(STATUS_TOPIC, doc, true);
}

bool componentsReady() {
  return componentStatus.motionSensor && componentStatus.RFID && componentStatus.scale &&
         componentStatus.network && componentStatus.time && componentStatus.motor;
}

void sendTelemetry(const FeedingContext& context, unsigned long durationSeconds, float weightConsumed) {
  StaticJsonDocument<384> doc;
  doc["camelUID"] = context.camelUID;
  doc["time"] = String(context.timestamp.hour) + ":" + String(context.timestamp.minute) + ":" + String(context.timestamp.second);
  doc["weightOfConsumedFood"] = weightConsumed;
  doc["consumptionTime"] = durationSeconds;
  doc["foodType"] = context.foodType;
  doc["amountDropped"] = context.foodDropped;
  doc["chambersRemaining"] = total_chambers;
  publishJson(TELEMETRY_TOPIC, doc);
}

void ensureMqttConnection() {
  if (client.connected()) {
    return;
  }

  while (!client.connected()) {
    const String clientId = String("camel-feeder-") + String(ESP.getChipId(), HEX) + String(random(0xFFFF), HEX);
    if (client.connect(clientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD)) {
      client.subscribe(COMMAND_TOPIC);
      publishEvent("mqtt_connected");
    } else {
      delay(2000);
    }
  }
}

void handleCommand(const JsonDocument& doc) {
  const char* command = doc["command"];
  if (!command) {
    return;
  }

  if (strcmp(command, "refill") == 0) {
    const int chambers = doc["chambers"] | total_chambers;
    if (chambers > 0) {
      total_chambers = chambers;
      isDispenserReady = componentsReady();
      if (isDispenserReady) {
        ctx.state = FeedingState::WaitingForMotion;
      }
      publishEvent("chambers_refilled", String(chambers));
      reportComponentStatus();
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, payload, length);
  if (err) {
    return;
  }

  if (strcmp(topic, COMMAND_TOPIC) == 0) {
    handleCommand(doc);
  }
}

bool captureCurrentTime(TimeLog& log) {
  getTime2(log.hour, log.minute, log.second, log.day, log.month, log.year);
  return log.hour != 0 && log.minute != 0 && log.second != 0 && log.day != 0 && log.month != 0 && log.year != 0;
}

bool planMeal(const TimeLog& log, String& foodType, float& foodDropped, int& motorCycles) {
  if (log.hour < 12) {
    foodType = "alfalfa";
    foodDropped = 2500;
    motorCycles = 1;
    return true;
  }

  if (log.hour < 15) {
    foodType = "alfalfa";
    foodDropped = 5000;
    motorCycles = 2;
    return true;
  }

  if (log.hour >= 15) {
    foodType = "barley";
    foodDropped = 2500;
    motorCycles = 1;
    return true;
  }

  return false;
}

void resetSession() {
  ctx = FeedingContext{};
  ctx.state = FeedingState::WaitingForMotion;
}

void finalizeSession() {
  const unsigned long durationSeconds = (millis() - ctx.sessionStartMs) / 1000;
  const float consumed = max(0.0f, ctx.weightStart - ctx.lastWeight);
  sendTelemetry(ctx, durationSeconds, consumed);
  publishEvent("camel_stopped_eating", ctx.camelUID);

  total_chambers = max(0, total_chambers - 1);
  if (total_chambers == 0) {
    isDispenserReady = false;
    publishEvent("chambers_empty");
  }

  reportComponentStatus();
  resetSession();
}

void setup() {
  Serial.begin(115200);
  randomSeed(micros());
  Serial.println("Setting up components...");

  connectToNetwork(WIFI_SSID, WIFI_PASS, componentStatus.network);
  espClient.setInsecure();
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setBufferSize(512);
  client.setCallback(mqttCallback);

  scaleSetup(componentStatus.scale);
  motionSetup(componentStatus.motionSensor);
  SPI.begin();
  setupRFID(componentStatus.RFID);
  setupMotor(componentStatus.motor);
  timeSetup2(componentStatus.time);

  if (componentsReady()) {
    isDispenserReady = true;
    resetSession();
  }

  reportComponentStatus();
}

void loop() {
  ensureMqttConnection();
  client.loop();

  if (!isDispenserReady || total_chambers <= 0) {
    delay(IDLE_DELAY_MS);
    return;
  }

  const unsigned long now = millis();

  switch (ctx.state) {
    case FeedingState::Idle:
      ctx.state = FeedingState::WaitingForMotion;
      break;

    case FeedingState::WaitingForMotion:
      if (readMotionSensor()) {
        ctx.sessionStartMs = now;
        ctx.lastActivityMs = now;
        publishEvent("motion_detected");
        ctx.state = FeedingState::WaitingForCamel;
      }
      break;

    case FeedingState::WaitingForCamel: {
      bool camelDetected = false;
      RFID(ctx.camelUID, camelDetected);
      if (camelDetected && ctx.camelUID.length() > 0) {
        publishEvent("camel_identified", ctx.camelUID);
        ctx.state = FeedingState::Dispensing;
      }
      break;
    }

    case FeedingState::Dispensing: {
      if (!captureCurrentTime(ctx.timestamp)) {
        publishEvent("time_unavailable");
        break;
      }

      String foodType;
      float foodDropped = 0;
      int motorCycles = 0;
      if (!planMeal(ctx.timestamp, foodType, foodDropped, motorCycles)) {
        publishEvent("no_meal_scheduled");
        resetSession();
        break;
      }

      motor(motorCycles);
      ctx.foodType = foodType;
      ctx.foodDropped = foodDropped;
      ctx.weightStart = readScale(WEIGHT_SAMPLE_COUNT, WEIGHT_SAMPLE_DELAY_MS);
      ctx.lastWeight = ctx.weightStart;
      ctx.lastActivityMs = now;
      publishEvent("camel_started_eating", ctx.camelUID);
      ctx.state = FeedingState::Monitoring;
      break;
    }

    case FeedingState::Monitoring: {
      const bool motionDetected = readMotionSensor();
      if (motionDetected) {
        ctx.lastActivityMs = now;
      }

      const float weight = readScale(WEIGHT_SAMPLE_COUNT, WEIGHT_SAMPLE_DELAY_MS);
      if (fabs(weight - ctx.lastWeight) > WEIGHT_CHANGE_THRESHOLD) {
        ctx.lastWeight = weight;
        ctx.lastActivityMs = now;
      }

      if ((now - ctx.lastActivityMs) >= INACTIVITY_TIMEOUT_MS) {
        finalizeSession();
      }
      break;
    }
  }

  delay(IDLE_DELAY_MS);
}
