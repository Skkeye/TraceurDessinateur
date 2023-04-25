

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
    } else {
      Console << "Commande inconnue" << endl;
    }
    Console.prompt();
    return;
  }

  // mode admin
  if (command == "help") {  // affiche l'aide
    Console << endl << endl << endl;
    printBanner();
    Console << endl << endl;
    Console << "Commandes:" << endl;
    Console << "help: affiche l'aide" << endl;
    Console << "log: affiche les messages du broker" << endl;
    Console << "nolog: ne plus afficher les messages du broker" << endl;
    Console << "gyrozero: recalibre la manette" << endl;
    Console << "home: met a zero le dessinateur et le recentre" << endl;
    Console << "stop: arrete le dessinateur" << endl;
    Console << "start: demarre le dessinateur" << endl;
    Console << "pause: met en pause le dessinateur" << endl;
    Console << "resume: reprend le dessinateur" << endl;
    Console << "restart: redemarre le serveur" << endl;
    Console << "admin: entre dans le mode admin" << endl;
    Console << "status: affiche le status des connexions" << endl;
    Console << "exit: quitte le mode admin" << endl;
  } else if (command == "gyrozero") {  // recalibre la manette
    adminClient.publish("admin", "gyrozero", RETAIN);
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
  } else if (command == "restart") {  // redemarre le serveur
    adminClient.publish("admin", "restart", RETAIN);
    delay(1000);
    rp2040.reboot();
  } else if (command == "status") {  // affiche le status des connexions
    Console << "Status des connexions:" << endl;
    Console << "  - Admin: " << (bAdminActive ? "active" : "desactive") << endl;
    Console << "  - Logging: " << (bAdminLogging ? "active" : "desactive") << endl;
    Console << "  - Clients: " << broker.clientsCount() << endl;
  } else {  // commande inconnue
    Console << "Commande inconnue" << endl;
  }
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
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  Console.begin(Serial);
  Console.setCallback(consoleCallback);
  printBanner();

  Console << endl << endl << endl;
  Console << "MQTT Broker PIT" << endl << endl;

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(server_ip, gateway, subnet);
  WiFi.softAP(SSID);

  Console << "Point d'acces WiFi configure" << endl;
  Console << "SSID: " << SSID << endl;
  Console << "IP: " << server_ip.toString() << ":" << PORT << endl;
  Console << "Gateway: " << gateway.toString() << endl;
  Console << "Subnet mask: " << subnet.toString() << endl << endl;

  // init du broker
  broker.begin();
  Console << "Broker initialise" << endl << endl;

  // init du Callback du client admin
  adminClient.setCallback(logCallback);
  adminClient.subscribe("inTopic");
  adminClient.subscribe("outTopic");
  adminClient.subscribe("admin");
}

void loop() {
  broker.loop();
  adminClient.loop();
  Console.loop();
//  very short blink
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1);
}
