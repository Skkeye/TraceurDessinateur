//#include <FreeRTOS.h>

#include "StepperWrapper.h"
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define NAME "Broker PIT"
#define PORT 1337
const IPAddress server_ip(192, 168, 144, 1);

DynamicJsonDocument doc(256);
char message[256] = "";

WiFiClient wifi_client;
PubSubClient client(wifi_client);

StepperWrapper stepper_x(0, 1, 2, 12, 12845);
StepperWrapper stepper_y(4, 5, 6, 13, 10752);
StepperWrapper stepper_z(8, 9, 10, 14, 1325);
ThreeAxisStepper steppers(&stepper_x, &stepper_y, &stepper_z);

void callback(char *topic, uint8_t *payload, unsigned int lenght)
{
  deserializeJson(doc, payload);

  int steps_x = (int)((short)doc["angle_x"] / 8) - 15;
  int steps_y = (int)((short)doc["angle_y"] / 8) - 15;
  int steps_z = (int)((short)doc["bouton"] / 8) - 15;
  // Serial.println(steps_x);
  // Serial.println(steps_y);
  // Serial.println(steps_z);

  steppers.move(steps_x, steps_y, steps_z);

  // Serial.print("Topic: ");
  // Serial.println(topic);
  // Serial.println();
  // Serial.println((short)doc["angle_x"]);
  // Serial.println((short)doc["angle_y"]);
  // Serial.println((short)doc["bouton"]);
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient"))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(NAME);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }

  client.setServer(server_ip, PORT);
  client.setCallback(callback);
  
  steppers.enable();
  delay(1500);
}

void loop()
{
	if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}
