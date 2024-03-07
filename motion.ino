
int motionSensor = D1;


void setup() {
  // put your setup code here, to run once:


Serial.begin(9600);


pinMode(LED_BUILTIN,OUTPUT);
digitalWrite(LED_BUILTIN, HIGH);


pinMode(motionSensor, INPUT);


delay(1500);


Serial.println("Motion sensor test.");
}


unsigned long currentTime;


const unsigned long motionCheckInterval = 150UL;
unsigned long previousMotionCheckTime = 0;


void loop() {
  // put your main code here, to run repeatedly:


currentTime = millis();


if (currentTime - previousMotionCheckTime >= motionCheckInterval){


int isMotionDetected = digitalRead(motionSensor);


  if (isMotionDetected == 0){
    
    Serial.println("Motion ended!");
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else {
    Serial.println("Motion detected1");
    digitalWrite(LED_BUILTIN, LOW);
  }


  previousMotionCheckTime = currentTime;
}
}