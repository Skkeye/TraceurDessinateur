/**
 * @file
 * @brief Code Manette
 *
 * Olivier David Laplante - 11 Avril 2023 - Creation
 */

#include "global.h"
#include "TDProto.h"

// #include <ArduinoJson.h>
// #include <PubSubClient.h>
#include <MPU6050_tockn.h>
#include <WiFi.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <esp_task_wdt.h>

// Configurations WiFi/Serveur/Capteurs
#define NAME "Manette PIT"
#define SSID "Dessinateur PIT"
#define ADMIN "Admin PIT"
#define ADDR "192.168.144.42"
#define PORT 1337
#define PIN_BUTTON 37

// Instantiation du Capteur
SemaphoreHandle_t xDataSemaphore;
MPU6050 xManetteIMU(Wire);
tData xData;

// Biais du IMU
float fOffsetX;
float fOffsetY;

/**
 * @brief Initialise la connexion au reseau WiFi
 */
void initWiFi()
{
  Serial.println("Connexion au controlleur...");
  // client.setServer(ADDR, PORT);
  // client.setCallback(callback);
  WiFi.mode(WIFI_AP_STA);
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
  
  /**
   * @brief Formatte les donnees brutes en un nombre (short) de 0-255
   */
  short sAngleX = ((128 + round(fAngleX * 2)) > 255) ? 255 : ((128 + round(fAngleX * 2)) < 0) ? 0 : (128 + round(fAngleX * 2));
  short sAngleY = ((128 + round(fAngleY * 2)) > 255) ? 255 : ((128 + round(fAngleY * 2)) < 0) ? 0 : (128 + round(fAngleY * 2));
  short sButtonValue = floor(iButtonValue / 16);

  xSemaphoreTake(xDataSemaphore, portMAX_DELAY);
  xData.u8AngleX = sAngleX;
  xData.u8AngleY = sAngleY;
  xData.u8ButtonValue = sButtonValue;
  xSemaphoreGive(xDataSemaphore);
}

/**
 * @brief Appelle les fonctions d'initialisation et demarre les taches
 */
void setup()
{
  Serial.begin(115200);
  xDataSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(xDataSemaphore);
  initWiFi();
  initIMU();

  xTaskCreatePinnedToCore(vDataTask, "vDataTask", 10000, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(vWiFiServerTask, "vWiFiServer", 10000, NULL, 1, NULL, 1);

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
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

// wifiserver task
void vWiFiServerTask(void *pvParameters)
{
  WiFiServer server(PORT);
  server.begin();
  while (1)
  {
    WiFiClient client = server.available();
    if (!client)
    {
      vTaskDelay(1);
      continue;
    }
    while (!client.available())
    {
      vTaskDelay(1);
    }
    String request = client.readStringUntil('>end>');
    parseData(request);
    xSemaphoreTake(xDataSemaphore, portMAX_DELAY);
    String response = buildData(xData);
    xSemaphoreGive(xDataSemaphore);
    client.print(response);
    client.flush();
    client.stop();
    Serial.println("Client disconnected");
    vTaskDelay(1);
  }
}

void loop() {
  
}
