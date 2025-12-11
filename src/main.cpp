#include <Arduino.h>
#include "Globals.h"
#include "Config.h"
#include "Pins.h"
#include "TaskFirebase.h"

void taskReadSensors(void *pv);
void taskMQTT(void *pv);
void taskHeater(void *pv);
void taskHeartbeat(void *pv);

SemaphoreHandle_t dataMutex;

float outsideTemp = 0;
float coolsideTemp = 0;
float heaterTemp = 0;
bool heaterOn = false;

float targetTemp = 25.0; // default until MQTT overrides

const char *WIFI_SSID = "Gimp";
const char *WIFI_PASS = "FC7KUNPX";

const char *MQTT_HOST = "broker.hivemq.com";
const int MQTT_PORT = 8883;
const char *MQTT_USER = "Tortoise";
const char *MQTT_PASS = "Hea1951Ter";

void setup()
{
  Serial.begin(115200);

  dataMutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(taskReadSensors, "Sensors", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(taskMQTT, "MQTT", 6144, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskHeater, "Heater", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(taskHeartbeat, "Log", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(taskFirebase, "Firebase", 6144, NULL, 1, NULL, 1);
}

void loop()
{
  // Empty — all work is done in FreeRTOS tasks
}