#ifndef SCALE_H
#define SCALE_H
#include <Arduino.h>
#include <HX711.h>

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = D0;
const int LOADCELL_SCK_PIN = D8;

HX711 scale;

void scaleSetup(bool &state) {
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(229.575758);
  scale.tare();               // reset the scale to 0
  state = true;
}

float readScale(uint8_t samples = 5, uint16_t sampleDelayMs = 40) {
  float total = 0.0f;
  if (samples == 0) {
    samples = 1;
  }
  for (uint8_t i = 0; i < samples; ++i) {
    total += scale.get_units(1);
    delay(sampleDelayMs);
  }
  return total / samples;
}

#endif
