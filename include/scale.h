#ifndef SCALE_H
#define SCALE_H

#include <Arduino.h>
#include "HX711.h"

void beginScale(HX711& scale, int doutPin, int sckPin);
void calibrateScale(HX711& scale, float calibrationFactor);
void displayScaleInfo(HX711& scale, const String& message);

#endif