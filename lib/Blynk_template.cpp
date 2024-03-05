#define BLYNK_TEMPLATE_ID      "MyTemplateID"
#define BLYNK_TEMPLATE_NAME    "MyTemplateName"
#define BLYNK_AUTH_TOKEN       "MyAuthToken"
//Here we are enabling output to Serial monitor of what Blynk is doing for debugging purposes.
#define BLYNK_PRINT Serial

//These lines below are specific to your hardware. For example, it you are working with NodeMCU, these lines will be different.
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

//wifi credentials
char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";


BlynkTimer timer; // Creating a timer object


void setup()
{

  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Setup a timer function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}


// This loop defines what happens when timer is triggered
// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
}

//In the code below we track when the device connects to the Blynk.Cloud with BLYNK_CONNECTED. When device successfully connects to the Blynk Cloud, we change the URL of an image to show in the widget and also assign a click URL
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/template-quick-setup");
}


// this is an example of reading sensor data
sensorData = analogRead(A0);
Blynk.virtualWrite(V5, sensorData);

//Hardware may send data to the Widgets over the Virtual Pin like this:
Blynk.virtualWrite(Vpin, "abc"); 
Blynk.virtualWrite(Vpin, 123); Blynk.virtualWrite(pin, 12.34); 
Blynk.virtualWrite(Vpin, "hello", 123, 12.34);

//
int virtual_pin_value = param.asInt(); or param.asStrng(); or param.asFloat();



void notifications(){
  //get alerts to start reloading the food and when its empty and when camel has finished
}

void loop()
{
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}