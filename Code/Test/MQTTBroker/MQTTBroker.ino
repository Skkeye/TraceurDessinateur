#include <WiFi.h>
#include <TinyMqtt.h>
#include <TinyConsole.h>
#include <ArduinoJson.h>

// configurations du broker
#define SSID "Broker PIT"
#define PORT 1337
#define RETAIN 10
IPAddress server_ip(192, 168, 144, 11);
IPAddress gateway(192, 168, 144, 1);
IPAddress subnet(255, 255, 255, 0);

// broker et client
MqttBroker broker(PORT);
MqttClient adminClient(&broker);
// TinyConsole Console;

// variables pour le mode admin
bool bAdminLogging = false;
bool bAdminActive = false;

/**
 * @brief Fonction qui affiche le logo du projet
 */
void printBanner() {
  // "TraceurDessinateur" en art ASCII
  Console << " ______                                    " << endl;
  Console << "(_) |                                      " << endl;
  Console << "    | ,_    __,   __   _          ,_       " << endl;
  Console << "  _ |/  |  /  |  /    |/  |   |  /  |      " << endl;
  Console << " (_/    |_/\\_/|_/\\___/|__/ \\_/|_/   |_/    " << endl;
  Console << endl;
  Console << "  ____                                                   " << endl;
  Console << " (|   \\             o                                    " << endl;
  Console << "  |    | _   ,   ,      _  _    __, _|_  _          ,_   " << endl;
  Console << " _|    ||/  / \\_/ \\_|  / |/ |  /  |  |  |/  |   |  /  |  " << endl;
  Console << "(/\\___/ |__/ \\/  \\/ |_/  |  |_/\\_/|_/|_/|__/ \\_/|_/   |_/" << endl;
  Console << endl;
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
      Console << "Mode admin active" << endl;
      Console.setPrompt("admin> ");
    } else {
      Console << "Commande inconnue" << endl;
    }
    Console.prompt();
    return;
  }

  // mode admin
  if (command == "help") {  // affiche l'aide
    Console << "help: affiche l'aide" << endl;
    Console << "log: affiche les messages du broker" << endl;
    Console << "nolog: ne plus afficher les messages du broker" << endl;
    Console << "recalibrate: recalibre la manette" << endl;
    Console << "home: met a zero le dessinateur et le recentre" << endl;
    Console << "stop: arrete le dessinateur" << endl;
    Console << "start: demarre le dessinateur" << endl;
    Console << "pause: met en pause le dessinateur" << endl;
    Console << "resume: reprend le dessinateur" << endl;
    Console << "restart: redemarre le serveur" << endl;
    Console << "admin: entre dans le mode admin" << endl;
    Console << "exit: quitte le mode admin" << endl;
  } else if (command == "recalibrate") {  // recalibre la manette
    adminClient.publish("admin", "recalibrate", RETAIN);
  } else if (command == "home") {  // recentre le dessinateur
    adminClient.publish("admin", "home", RETAIN);
  } else if (command == "stop") {  // arrete le dessinateur
    adminClient.publish("admin", "stop", RETAIN);
  } else if (command == "start") {  // demarre le dessinateur
    adminClient.publish("admin", "start", RETAIN);
  } else if (command == "pause") {  // met en pause le dessinateur
    adminClient.publish("admin", "pause", RETAIN);
  } else if (command == "resume") {  // reprend le dessinateur
    adminClient.publish("admin", "resume", RETAIN);
  } else if (command == "log") {  // affiche les messages du broker
    bAdminLogging = true;
  } else if (command == "nolog") {  // ne plus afficher les messages du broker
    bAdminLogging = false;
  } else if (command == "exit") {  // quitte le mode admin
    bAdminActive = false;
    Console << "Mode admin desactive" << endl;
    Console.setPrompt("> ");
  } else if (command == "restart") {  // redemarre le serveur
    // TODO: redemarrer le serveur
  } else {  // commande inconnue
    Console << "Commande inconnue" << endl;
  }
  Console.prompt();
}

/**
 * @brief Callback appele lorsqu'un message est recu
 *
 * @param client le client qui a recu le message
 * @param topic le topic sur lequel le message a ete recu
 * @param payload le message recu
 * @param lenght la taille du message
 */
void logCallback(const MqttClient *client, const Topic &topic, const char *payload, unsigned int lenght) {
  if (bAdminLogging == true) {
   Console << topic.str() << ": " << payload << endl;
  }
}

/**
 * @brief Fonction d'initialisation
 */
void setup() {
  delay(10000);
  Serial.begin(115200);

  Console.begin(Serial);
  Console.title("PIT Broker");
  Console.setCallback(consoleCallback);
  printBanner();

  Console << endl
         << endl
         << endl
         << endl << endl;
  Console << "MQTT Broker PIT" << endl
         << endl;

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(server_ip, gateway, subnet);
  WiFi.softAP(SSID);

  Console << "Point d'acces WiFi configure" << endl;
  Console << "SSID: " << SSID << endl;
  Console << "IP: " << server_ip.toString() << ":" << PORT << endl;
  Console << "Gateway: " << gateway.toString() << endl;
  Console << "Subnet mask: " << subnet.toString() << endl
          << endl;

  // init du broker
  broker.begin();
  Console << "Broker initialise" << endl
          << endl;

  // init du Callback du client admin
  adminClient.setCallback(logCallback);
}

void loop() {
  broker.loop();
  adminClient.loop();
  Console.loop();
}
