
#pragma once

#include <WiFi.h>


class ManetteServer {
private:
  IPAddress gateway_ip;
  WiFiServer server;
  WiFiClient client;

public:
  ManetteServer(IPAddress ip_addr, uint16_t port, String name);
}