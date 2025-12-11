#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <freertos/semphr.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

extern SemaphoreHandle_t dataMutex;

extern float outsideTemp;
extern float coolsideTemp;
extern float heaterTemp;
extern float targetTemp;

extern bool heaterOn;

// MQTT globals
extern WiFiClientSecure wifi;
extern PubSubClient mqtt;

// Task functions
void reconnectMQTT();
void taskMQTT(void *pv);

#endif