#include <WiFi.h>
#include <SoftwareSerial.h>

#include "StepperWrapper.h"

//#define IS_AP
#define NAME "Dessinateur"
#define SSID "Jean 3-16"
#define PASSWD "Pilot2014"

//  (dir_pin, step_pin, en_pin, home_pin, max_steps)
StepperWrapper stepper_x(0, 1, 2, 12, 12845);
StepperWrapper stepper_y(4, 5, 6, 13, 10752);
StepperWrapper stepper_z(8, 9, 10, 14, 1325);
ThreeAxisStepper steppers(&stepper_x, &stepper_y, &stepper_z);

IPAddress apIP(172, 217, 28, 1);
WiFiServer server(1337);
WiFiClient client;
String msg = "";

void setup()
{
	Serial.begin();
  steppers.enable();
  // #ifdef IS_AP
  // WiFi.mode(WIFI_AP);
  // WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  // WiFi.softAP(NAME);
  // #else
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(SSID, PASSWD);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print('.');
  // }
  // Serial.println("");
  // Serial.print("Connected to ");
  // Serial.println(SSID);
  // Serial.print("IP address: ");
  // Serial.println(WiFi.localIP());
  // #endif

  // server.begin();
}

void loop()
{
	delay(1000);
  steppers.move(10, 5, 3);
  delay(1000);
  steppers.move(-5, 3, -10);
  // client = server.available();
  // if(!client) return;
  // while(client.connected()) {
  //   // if(client.available()) Serial.print((char)client.read());
  //   // if(Serial.available()) client.print((char)Serial.read());

  //   if(client.available()) {
  //     msg = client.readStringUntil('\n');
  //     Serial.println(msg);
  //     // if(msg.startsWith("togglepin")) {
  //     //   int pin_num = msg.charAt(10) - '0';
  //     //   char pin_state = msg.charAt(12);
  //     //   Serial.print(pin_num);
  //     //   Serial.println(pin_state);
  //     //   if(pin_num < 2 && pin_num > -1) {
  //     //     digitalWrite(pin_num, pin_state == '1');
  //     //   } 
  //     // }
  //     if(msg.startsWith("step")) {
  //       int steps = msg.charAt(5) - '0';
  //       char dir = msg.charAt(7) - '0';
  //       for(int i = 0; i < steps; i++) {
          
  //       }
  //     }
  //   }

  // }
  // client.flush();
}
