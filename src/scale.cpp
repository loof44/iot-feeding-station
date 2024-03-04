#include "scale.h"

void beginScale(HX711& scale, int doutPin, int sckPin) {
   scale.begin(doutPin, sckPin);
}

void calibrateScale(HX711& scale, float calibrationFactor) {
  scale.set_scale(calibrationFactor); 
  scale.tare();  
}

void displayScaleInfo(HX711& scale, const String& message){
    Serial.println(message);

    Serial.print("read: \t\t");
    Serial.println(scale.read()); 

    Serial.print("read average: \t\t");
    Serial.println(scale.read_average(20));  

    Serial.print("get value: \t\t");
    Serial.println(scale.get_value(5));

    Serial.print("get units: \t\t");
    Serial.println(scale.get_units(5), 1); 
}