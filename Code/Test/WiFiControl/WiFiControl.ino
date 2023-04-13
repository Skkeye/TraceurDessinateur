#include <ArduinoJson.h>

#include <WiFi.h>
#include <PubSubClient.h>

#define NAME "Broker PIT"
#define PORT 1337
const IPAddress server_ip(192, 168, 144, 1);
// CommandPacket recalibration_packet;

DynamicJsonDocument doc(256);
char message[256] = "";

WiFiClient wifi_client;
PubSubClient client(wifi_client);
// ManetteServer my_server(my_ip, PORT, NAME);

void callback(char *topic, uint8_t *payload, unsigned int lenght)
{
  deserializeJson(doc, payload);
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.println();
  Serial.println((short)doc["angle_x"]);
  Serial.println((short)doc["angle_y"]);
  Serial.println((short)doc["bouton"]);
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
