/**
 * @file
 * @brief Code Manette
 *
 * Olivier David Laplante - 11 Avril 2023 - Creation
 */


#include <MsgPacketizer.h>
#include <MPU6050_tockn.h>
#include <WiFi.h>
#include <Wire.h>

// Definitions pour les packets WiFi
#include "PacketDefinitions.h"

// Configurations WiFi/Serveur/Capteurs
#define SSID "Nugget"
#define PASS "POGGERS7"
#define ADDR "192.168.144.1"
#define PORT 1337
#define PIN_BUTTON 37

// Instantiation du Capteur/ClientWiFi/Packet
MPU6050 xManetteIMU(Wire);
WiFiClient client;
DataPacket xIMUDataPacket;

// Biais du IMU
float fOffsetX;
float fOffsetY;

/**
 * @brief Assure la connexion WiFi et Serveur
 */
void assertConnection()
{
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
  }
  if (client.connected())
    return;
  connectToServer();
}

/**
 * @brief Initialise la connexion au reseau WiFi
 */
void initWiFi()
{
  Serial.println("Connexion au controlleur...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
  }
  Serial.println("OK");
  Serial.println();
}

/**
 * @brief Initialise la connexion au Serveur
 */
void connectToServer()
{
  Serial.println("Connexion au serveur...");
  while (client.connect(ADDR, PORT))
  {
    Serial.print('.');
    delay(500);
  }
  Serial.println("OK");
}

/**
 * @brief Calcule les biais du IMU
 */
void recalibrateIMU()
{
  float fCumulX = 0;
  float fCumulY = 0;
  for(int i = 0; i < 3000; i++) {
    xManetteIMU.update();
    fCumulX += xManetteIMU.getAccAngleX();
    fCumulY += xManetteIMU.getAccAngleY();
  }
  fOffsetX = fCumulX / 3000;
  fOffsetY = fCumulY / 3000;  
}

/**
 * @brief Initialise le IMU
 */
void initIMU()
{
  Wire.begin();
  xManetteIMU.begin();
  recalibrateIMU();
}

/**
 * @brief Met a jour les donnees du IMU et du bouton
 */
void updateDataIMU()
{
  xManetteIMU.update();
  float fAngleX = xManetteIMU.getAccAngleX() - fOffsetX;
  float fAngleY = xManetteIMU.getAccAngleY() - fOffsetY;
  int iButtonValue = analogRead(PIN_BUTTON);
  // Serial.println(round(fAngleX));
  // Serial.println(round(fAngleY));
  // Serial.println(iButtonValue);
  
  /**
   * @brief Formatte les donnees brutes en un nombre (short) de 0-255
   */
  short sAngleX = ((128 + round(fAngleX * 2)) > 255) ? 255 : ((128 + round(fAngleX * 2)) < 0) ? 0 : (128 + round(fAngleX * 2));
  short sAngleY = ((128 + round(fAngleY * 2)) > 255) ? 255 : ((128 + round(fAngleY * 2)) < 0) ? 0 : (128 + round(fAngleY * 2));
  short sButtonValue = floor(iButtonValue / 16);
  // Serial.println(sAngleX);
  // Serial.println(sAngleY);
  // Serial.println(sButtonValue);
  xIMUDataPacket.sX = sAngleX;
  xIMUDataPacket.sY = sAngleY;
  xIMUDataPacket.sZ = sButtonValue;
}

/**
 * @brief Initialise le Packeteur et la fonction de rappel "subscribe"
 *
 * @callback Si le packet demande un recalibrage, appeler "recalibrateIMU()"
 */
void initPacketHandler()
{
  MsgPacketizer::subscribe(client, INDEX_MOSI,
    [&](const CommandPacket& packet) {
      if(packet.bRecalibrate) recalibrateIMU();
    });
  MsgPacketizer::publish(client, INDEX_MISO, xIMUDataPacket); 
}

/**
 * @brief Appelle les fonctions d'initialisation
 */
void setup()
{
  Serial.begin(115200);
  initWiFi();
  connectToServer();
  initIMU();
  initPacketHandler();  
}

/**
 * @brief Cycle principal du programme
 *
 * 1. Mettre a jour les donnees
 * 2. Assurer la connexion WiFi et Serveur
 * 3. Mettre a jour le PacketHandler
 */
void loop()
{
  updateDataIMU();
  assertConnection();
  MsgPacketizer::update();
}
