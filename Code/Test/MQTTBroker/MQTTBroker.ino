#include <WiFi.h>
#include <TinyMqtt.h>


#define SSID "Broker PIT"
#define PORT 1337
#define RETAIN 10
IPAddress server_ip(192, 168, 144, 11);
IPAddress gateway(192, 168, 144, 1);
IPAddress subnet(255, 255, 255, 0);

MqttBroker broker(PORT);

void setup()
{
	Serial.begin(115200);
  Console.begin(Serial);
  Console.title("PIT Broker");
  Console << endl << endl << endl << endl << endl;
  Console << "MQTT Broker PIT" << endl << endl;

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(server_ip, gateway, subnet);
  WiFi.softAP(SSID);
  Console << "Point d'acces WiFi configure" << endl;
  Console << "SSID: " << SSID << endl;
  Console << "IP: " << server_ip.toString() << endl;
  Console << "Gateway: " << gateway.toString() << endl;
  Console << "Subnet mask: " << subnet.toString() << endl << endl;

  broker.begin();
  Console << "MQTT Broker demarre" << endl << endl;
}

void loop()
{
	broker.loop();
}
