/**
 * @file AllInOneDessinateur.ino
 * @brief Fichier principal du dessinateur
 * @date 2023-04-22
 *
**/

//#include <wiring_private.h>
#include "StepperWrapper.h"
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>


// Definitions de constantes
#define NAME "Broker PIT"
#define PORT 1337
const IPAddress server_ip(192, 168, 144, 1);

// Variables et instances globales
DynamicJsonDocument doc(256);
char message[256] = "";
WiFiClient wifi_client;
PubSubClient client(wifi_client);
// Steppers
StepperWrapper stepper_x(0, 1, 2, 12, 12845 * 4);
StepperWrapper stepper_y(4, 5, 6, 13, 10752 * 4);
StepperWrapper stepper_z(8, 9, 10, 14, 1325 * 4, true);
ThreeAxisStepper steppers(&stepper_x, &stepper_y, &stepper_z);
bool paused = false;
// Variables pour les deltas
int deltaX = 0;
int deltaY = 0;
int deltaZ = 0;

/**
 * @brief Fonction qui gère les messages admin
 *
 * @param topic : topic du message
 * @param payload : payload du message
 * @param lenght : longueur du payload
 */
void adminMessage(char *topic, uint8_t *payload, unsigned int lenght)
{
  if (strcmp((char *)payload, "home") == 0) // home : mise a zero de la position
  {
    paused = true;
    steppers.home();
    paused = false;
  }
  else if (strcmp((char *)payload, "start") == 0) // start : demarrage des moteurs
  {
    steppers.enable();
  }
  else if (strcmp((char *)payload, "stop") == 0) // stop : arret des moteurs
  {
    steppers.disable();
  }
  else if (strcmp((char *)payload, "pause") == 0) // pause : pause les deplacements
  {
    paused = true;
  }
  else if (strcmp((char *)payload, "resume") == 0) // resume : reprend les deplacements
  {
    paused = false;
  }
  else if (strcmp((char *)payload, "restart") == 0) // restart : redemarre le RP2040
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    rp2040.reboot();
  }
}

/**
 * @brief Fonction qui fait clignoter la LED
 *
 */
void vShortBlink()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1);
}

/**
 * @brief Fonction qui gère les messages MQTT
 *
 * @param topic : topic du message
 * @param payload : payload du message
 * @param lenght : longueur du payload
 */
void callback(char *topic, uint8_t *payload, unsigned int lenght)
{
  vShortBlink();  // blink led
  
  // Si le message est un message admin
  if (strcmp(topic, "admin") == 0)
  {
    adminMessage(topic, payload, lenght);
    return;
  }
  if (paused) // Si le dessinateur est en pause on ne fait rien
  {
    return;
  }
  deserializeJson(doc, payload); // On deserialise le message

  // On calcule les deltas
  deltaY = (int)((short)doc["angle_x"] / 2) - 63;
  deltaX = (int)((short)doc["angle_y"] / 2) - 63;
  deltaZ = (int)((short)doc["bouton"] / 2) - 63;
  
  client.flush();
}

/**
 * @brief Fonction qui fait tourner les moteurs
 * 
 */
void runSteppers() {
  unsigned long time = millis();
  steppers.move(deltaX, deltaY, deltaZ);
  delay((10 > (millis() - time)) ? (10 - (millis() - time)) : 0); // On attend jusqu'a 10ms pour avoir un delai constant
}

/**
 * @brief Fonction qui reconnecte le client MQTT
 * 
 */
void reconnect()
{
  // Boucle jusqu'a ce que le client se connecte
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");

    // essaye de se connecter
    if (client.connect("arduinoClient"))
    {
      Serial.println("connected");

      // Une fois connecte on s'abonne aux topics
      client.subscribe("inTopic");
      client.subscribe("admin");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      // On attend 5 secondes avant de recommencer
      delay(5000);
    }
  }
}

/**
 * @brief Fonction qui setup le dessinateur
 * 
 */
void setup()
{
  Serial.begin(115200); // On initialise le port serie
  pinMode(LED_BUILTIN, OUTPUT); // On initialise la LED builtin

  // On initialise le client MQTT 
  client.setServer(server_ip, PORT); 
  client.setCallback(callback); 

  // On se connecte au WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(NAME);

  // on active les moteurs
  steppers.enable();

  // On met les moteurs a zero
  steppers.home();
}

void loop()
{
	if (!client.connected()) // Si le client n'est pas connecte on se reconnecte
  {
    reconnect();
  }
  client.loop(); // On fait tourner le client MQTT
  runSteppers(); // On fait tourner les moteurs 
}
