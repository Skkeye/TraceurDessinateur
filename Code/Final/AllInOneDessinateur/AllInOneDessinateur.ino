#include "global.h"
#include <WiFi.h>


#define NAME "Dessinateur PIT"
#define SSID "Manette PIT"
#define ADMIN "Admin PIT"
#define PORT 1337
const IPAddress server_ip(192, 168, 144, 24);

TaskHandle_t xServoTask();
TaskHandle_t xWiFiTask();

WiFiClient wifi_client;

StepperWrapper stepper_x(0, 1, 2, 12, 12845 * 4, true); // dir, step, enable, limit, max_steps, flip_dir
StepperWrapper stepper_y(4, 5, 6, 13, 10752 * 4); // dir, step, enable, limit, max_steps
StepperWrapper stepper_z(8, 9, 10, 14, 1325 * 4); // dir, step, enable, limit, max_steps
ThreeAxisStepper steppers(&stepper_x, &stepper_y, &stepper_z);
Data_t xData;
bool paused = false;

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ADMIN);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  
  // start the tasks
  xTaskCreate(vStepperTask, "StepperTask", 10000, NULL, 1, NULL);
  xTaskCreate(vWiFiServerTask, "WiFiServerTask", 10000, NULL, 2, NULL);
}

// create a task to handle the stepper
void vStepperTask(void *pvParameters)
{
  steppers.enable();
  while (1)
  {
    if (!paused)
    {
      int steps_x = (int)xData.u8AngleX - 127;
      int steps_y = (int)xData.u8AngleY - 127;
      int steps_z = (int)xData.u8ButtonValue - 127;
      steppers.move(steps_x, steps_y, steps_z);
    }
    vTaskDelay(1);
  }
}

// create a task to handle the wifi server
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
    String request = client.readStringUntil('\r');
    parseData(request, xData, paused, &steppers);
    String response = ">ack>>end>";
    client.print(response);
    client.flush();
    client.stop();
    vTaskDelay(1);
  }
}

void loop()
{

}
