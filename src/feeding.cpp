#include <Arduino.h>

struct FeedingPlan {
  String camelUID;
  String foodType;
  float foodAmountKG;
  int feedingTimesPerDay;
  int feedingHour[10];
  int feedingMinute[10];  
};

void setup(){
    ...
    // Setup callbacks for Blynk
    BLYNK_WRITE(V1) {
        FeedingPlan.plan.camelUID = param.asStr();
    }
    BLYNK_WRITE(V2) {
        FeedingPlan.plan.foodType = param.asStr();
    }

    BLYNK_WRITE(V3) {
        FeedingPlan.plan.foodAmountKG = param.asFloat();
    }

    BLYNK_WRITE(V4) {
        FeedingPlan.plan.feedingTimesPerDay = param.asInt();
    }
    for(int i=0; i<FeedingPlan.plan.feedingTimes; i++){
        BLYNK_WRITE(V5+i) { // Assuming feeding hour starts from V5 and up to V4+FeedingTimes
            String time_str = param.asStr();
            sscanf(time_str.c_str(), "%02d:%02d", &FeedingPlan.plan.feedingHour[i], &FeedingPlan.plan.feedingMinute[i]);
        }
    }
}


void loop()
{
    ...
        for(int i=0; i<FeedingPlan.plan.feedingTimesPerDay; i++){
            if (currentTime.hour == FeedingPlan.plan.feedingHour[i] && currentTime.minute == FeedingPlan.plan.feedingMinute[i]){
                motor(1);  //assumed that motor(1) drops the food. You may need to adjust this
                isFoodDropped = true;
                Serial.println("Dropping " + String(FeedingPlan.plan.foodAmountKG) + "KG of "+ FeedingPlan.plan.foodType +"for camel with ID: " + camelUID);
                foodType = FeedingPlan.plan.foodType;
                foodDropped = FeedingPlan.plan.foodAmountKG;
            }
        }
    ...
}
