#include <WiFi.h>
#include <Arduino.h>

#define SSID "The Nutte Sacc"

// WiFiServer server(1337);
WiFiClient client;

void setup()
{
	Serial.begin(115200);
	Serial.print("Hi");

	WiFi.mode(WIFI_STA);
	WiFi.begin("Nugget","POGGERS7");
	while(WiFi.status() != WL_CONNECTED){
		Serial.print('.');
		delay(200);		
	}
	Serial.println("Connected!");
	Serial.println(WiFi.localIP());
	// server.begin();
}

void loop()
{
	// client = server.available();
	// Connect to the server
  Serial.println("Connecting to server...");
  while (!client.connect("httpbin.org", 80)) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("connected");
  
  // Send a GET request to the server
  client.println("GET /get?arduino=true HTTP/1.1");
	client.println("Cookie: No cookie for you >:3");
	client.println("Host: httpbin.org");
  client.println("Connection: close");
  client.println();
  
  // Read the server's response and print it to the serial monitor
  Serial.println("Response:");
  String content = client.readString();
	while(content = ""){
  	content = client.readString();
	}
  Serial.println(content);
  client.stop();
  
  // Wait 10 seconds before sending another request
  delay(10000);
}
