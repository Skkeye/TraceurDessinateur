/**
 * @file
 * @brief Code Manette
 *
 * Olivier David Laplante - 11 Avril 2023 - Creation
 */


#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <MPU6050_tockn.h>
#include <WiFi.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_task_wdt.h>

// Configurations WiFi/Serveur/Capteurs
#define SSID "Broker PIT"
#define ADDR "192.168.144.1"
#define PORT 1337
#define PIN_BUTTON 37

// Instantiation du Capteur/ClientWiFi/JSON
MPU6050 xManetteIMU(Wire);
WiFiClient wifi_client;
PubSubClient client(wifi_client);
DynamicJsonDocument doc(256);
char message[256] = "";

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
  while (!client.connected()) {
    connectToServer();
  };
}

/**
 * @brief Initialise la connexion au reseau WiFi
 */
void initWiFi()
{
  Serial.println("Connexion au controlleur...");
  client.setServer(ADDR, PORT);
  client.setCallback(callback);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID);
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

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      client.subscribe("outTopic");
      client.subscribe("admin");
    } else {

      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      delay(5000);
    }
    
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
  doc["angle_x"] = sAngleX;
  doc["angle_y"] = sAngleY;
  doc["bouton"] = sButtonValue;
  serializeJson(doc, message, 256);
  client.publish("inTopic", message);
  client.flush();
}

/**
 * @brief Callback pour le recalibrage du IMU sur un message MQTT admin
 */
void callback(char *topic, uint8_t *payload, unsigned int lenght)
{
  // "admin" -> "recalibrate" -> recalibrateIMU()
  if (strcmp(topic, "admin") == 0)
  {
    if (strcmp((char *)payload, "gyrozero") == 0)
    {
      recalibrateIMU();
      Serial.println("Recalibration du IMU");
    } else if (strcmp((char *)payload, "restart") == 0) {
      delay(1000);
      esp_restart();
    }
  }
}

/**
 * @brief Appelle les fonctions d'initialisation et demarre les taches
 */
void setup()
{
  Serial.begin(115200);
  initWiFi();
  connectToServer();
  initIMU();

  xTaskCreatePinnedToCore(vDataTask, "vDataTask", 10000, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(vMQTTTask, "vMQTTTask", 10000, NULL, 1, NULL, 1);

  esp_task_wdt_delete(NULL);
  esp_task_wdt_deinit();  
}

/**
 * @brief Appelle les fonctions de mise a jour du IMU et du bouton
 */
void vDataTask(void *pvParameters)
{
  
  while (1)
  {
    updateDataIMU();
    vTaskDelay(pdMS_TO_TICKS(100));
    
  }
}

/**
 * @brief Appelle les fonctions de connexion au serveur
 */
void vMQTTTask(void *pvParameters)
{
  
  while (1)
  {
    if (!client.connected())
    {
      connectToServer();
    }
    client.loop();
    vTaskDelay(30);
  }
}

void loop() {
  
}
