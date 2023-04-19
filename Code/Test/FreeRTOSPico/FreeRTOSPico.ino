#include <iostream>
#include <pico/multicore.h>
#include <WiFi.h>
#include <TinyConsole.h>
#include <CircularBuffer.h>
#include "TDProto.h"

#define SSID "admin"
#define PASSWORD "admin123"
#define PORT 1337
// IPAddresses 
const IPAddress gateway(192, 168, 144, 1);
const IPAddress subnet(255, 255, 255, 0);
const IPAddress server_ip(192, 168, 144, 2);


WiFiServer server(PORT);
WiFiClient admin_client;
//TinyConsole Console;

// variables pour le mode admin
bool bAdminLogging = false;
bool bAdminActive = false;

/**
 * @brief Fonction qui affiche le logo du projet
 */
void printBanner() {
  // "TraceurDessinateur" en art ASCII
  Console << " ______                                    " << std::endl;
  Console << "(_) |                                      " << std::endl;
  Console << "    | ,_    __,   __   _          ,_       " << std::endl;
  Console << "  _ |/  |  /  |  /    |/  |   |  /  |      " << std::endl;
  Console << " (_/    |_/\\_/|_/\\___/|__/ \\_/|_/   |_/    " << std::endl;
  Console << std::endl;
  Console << "  ____                                                   " << std::endl;
  Console << " (|   \\             o                                    " << std::endl;
  Console << "  |    | _   ,   ,      _  _    __, _|_  _          ,_   " << std::endl;
  Console << " _|    ||/  / \\_/ \\_|  / |/ |  /  |  |  |/  |   |  /  |  " << std::endl;
  Console << "(/\\___/ |__/ \\/  \\/ |_/  |  |_/\\_/|_/|_/|__/ \\_/|_/   |_/" << std::endl;
  Console << std::endl;
}


/**
 * @brief Gere les commandes de l'admin
 *
 * @param command le texte de la commande
 */
void consoleCallback(const std::string &command) {
  if (!bAdminActive) {
    if (command == "admin") {  // entre dans le mode admin
      bAdminActive = true;
      Console << "Mode admin active" << std::endl;
    } else {
      Console << "Commande inconnue" << std::endl;
    }
    Console.prompt();
    return;
  }

  // mode admin
  if (command == "help") {  // affiche l'aide
    Console << std::endl << std::endl << std::endl;
    printBanner();
    Console << std::endl << std::endl;
    Console << "Commandes:" << std::endl;
    Console << "help: affiche l'aide" << std::endl;
    Console << "log: affiche les messages du broker" << std::endl;
    Console << "nolog: ne plus afficher les messages du broker" << std::endl;
    Console << "gyrozero: recalibre la manette" << std::endl;
    Console << "home: met a zero le dessinateur et le recentre" << std::endl;
    Console << "stop: arrete le dessinateur" << std::endl;
    Console << "start: demarre le dessinateur" << std::endl;
    Console << "pause: met en pause le dessinateur" << std::endl;
    Console << "resume: reprend le dessinateur" << std::endl;
    Console << "restart: redemarre le serveur" << std::endl;
    Console << "admin: entre dans le mode admin" << std::endl;
    Console << "exit: quitte le mode admin" << std::endl;
  } else if (command == "gyrozero") {  // recalibre la manette
    admin_client.println(">cmd>gyrozero>end>");
  } else if (command == "home") {  // recentre le dessinateur
    admin_client.println(">cmd>home>end>");
  } else if (command == "stop") {  // arrete le dessinateur
    admin_client.println(">cmd>stop>end>");
  } else if (command == "start") {  // demarre le dessinateur
    admin_client.println(">cmd>start>end>");
  } else if (command == "pause") {  // met en pause le dessinateur
    admin_client.println(">cmd>pause>end>");
  } else if (command == "resume") {  // reprend le dessinateur
    admin_client.println(">cmd>resume>end>");
  } else if (command == "log") {  // affiche les messages du broker
    bAdminLogging = true;
  } else if (command == "nolog") {  // ne plus afficher les messages du broker
    bAdminLogging = false;
  } else if (command == "exit") {  // quitte le mode admin
    bAdminActive = false;
    Console << "Mode admin desactive" << std::endl;
  } else if (command == "restart") {  // redemarre le serveur
    Console << "Redemarrage du serveur" << std::endl;
    admin_client.println(">cmd>restart>end>");
    delay(1000);
    rp2040.reboot();
  } else if (command == "status") {  // affiche le status des connexions
    Console << "Status des connexions:" << std::endl;
    Console << "Admin: " << (bAdminActive ? "active" : "inactif") << std::endl;
    Console << "Logging: " << (bAdminLogging ? "active" : "inactif") << std::endl;
  } else {  // commande inconnue
    Console << "Commande inconnue" << std::endl;
  }
}

/**
 * @brief Fonction d'initialisation
 */
void setup() {
  Serial.begin(115200);
  multicore_launch_core1(core1_entry);

  Console.begin(Serial);
  Console.setCallback(consoleCallback);
  printBanner();

  Console << std::endl << std::endl << std::endl;
  Console << "Admin PIT" << std::endl << std::endl;

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(server_ip, gateway, subnet);
  WiFi.softAP(SSID);

  Console << "Point d'acces WiFi configure" << std::endl;
  Console << "SSID: " << SSID << std::endl;
  Console << "IP: " << server_ip.toString() << ":" << String(PORT) << std::endl;
  Console << "Gateway: " << gateway.toString() << std::endl;
  Console << "Subnet mask: " << subnet.toString() << std::endl << std::endl;

}

void loop() {
  Console.loop();
}

void core1_entry(void)
{
  CircularBuffer<tData, 100> xDataBuffer;
  CircularBuffer<eCommand, 100> xCommandBuffer;
  // init wifi ap
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(server_ip, gateway, subnet);
  WiFi.softAP(SSID, PASSWORD);
  // init server
  WiFiServer server(PORT);
  server.begin();
  // init client
  WiFiClient client[10];
  while (true)
  {
    // listen for incoming clients
    for (int i = 0; i < 10; i++)
    {
      client[i] = server.available();
      if (client[i])
      {
        String currentLine = ""; // make a String to hold incoming data from the client
        while (client[i].connected())
        { // loop while the client's connected
          if (client[i].available())
          { // if there's bytes to read from the client,
            // read until ">end>" is found
            // String request = client[i].readStringUntil('>end>'); like this but it doesn't work
            String request = "";
            while (client[i].available())
            {
              char c = client[i].read();
              request += c;
              if (request.endsWith(">end>"))
              {
                break;
              }
            }
            if (request.startsWith(">data>"))
            {
              // data
              tData data;
              data.u8AngleX = request.substring(request.indexOf("angle_x:") + 8, request.indexOf(";")).toInt();
              data.u8AngleY = request.substring(request.indexOf("angle_y:") + 8, request.indexOf(";", request.indexOf("angle_y:"))).toInt();
              data.u8ButtonValue = request.substring(request.indexOf("bouton:") + 7, request.indexOf(";", request.indexOf("bouton:"))).toInt();
              xDataBuffer.unshift(data);
            }
            else if (request.startsWith(">ack>"))
            {
              // ack
            }
            else if (request.startsWith(">req>"))
            {
              // req
              if (request.startsWith(">req>data>"))
              {
                // req data
                if (xDataBuffer.isEmpty() == false)
                {
                  tData data = xDataBuffer.pop();
                  String sData = buildData(data);
                  client[i].print(sData);
                }
              }
              else if (request.startsWith(">req>cmd>"))
              {
                // req cmd
                if (xCommandBuffer.isEmpty() == false)
                {
                  eCommand cmd = xCommandBuffer.pop();
                  String sCmd = ">cmd>" + String(cmd) + ">end>";
                  client[i].print(sCmd);
                }
              }
            }
            else if (request.startsWith(">cmd>"))
            {
              // cmd
              eCommand cmd = (eCommand)request.substring(request.indexOf(">cmd>") + 5, request.indexOf(">end>")).toInt();
              xCommandBuffer.unshift(cmd);
            }
            // clear the string for next input:
            request = "";
            // give the web browser time to receive the data
            delay(1);
            client[i].flush();

            // close the connection:
            client[i].stop();
            break;
          }
        }
      }
    }
  }
}
