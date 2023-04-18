//#include <wiring_private.h>
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
bool paused = false;

// callback for admin messages (not in json)
void adminMessage(char *topic, uint8_t *payload, unsigned int lenght)
{
  // possible commands: home, start, stop, pause, resume, restart
  if (strcmp((char *)payload, "home") == 0)
  {
    paused = true;
    steppers.home();
    paused = false;
  }
  else if (strcmp((char *)payload, "start") == 0)
  {
    steppers.enable();
  }
  else if (strcmp((char *)payload, "stop") == 0)
  {
    steppers.disable();
  }
  else if (strcmp((char *)payload, "pause") == 0)
  {
    paused = true;
  }
  else if (strcmp((char *)payload, "resume") == 0)
  {
    paused = false;
  }
  else if (strcmp((char *)payload, "restart") == 0)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    rp2040.reboot();
  }
}

void vShortBlink()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1);
}

void callback(char *topic, uint8_t *payload, unsigned int lenght)
{
  vShortBlink();
  // check if admin message
  if (strcmp(topic, "admin") == 0)
  {
    adminMessage(topic, payload, lenght);
    return;
  }
  if (paused)
  {
    return;
  }
  deserializeJson(doc, payload);

  int steps_x = (int)((short)doc["angle_x"] / 2) - 63;
  int steps_y = (int)((short)doc["angle_y"] / 2) - 63;
  int steps_z = (int)((short)doc["bouton"] / 2) - 63;

  steppers.move(steps_y, steps_x, steps_z);
  client.flush();
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
      //client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
      client.subscribe("admin");
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
  pinMode(LED_BUILTIN, OUTPUT);
  client.setServer(server_ip, PORT);
  client.setCallback(callback);
  WiFi.mode(WIFI_STA);
  WiFi.begin(NAME);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }

  
  
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
