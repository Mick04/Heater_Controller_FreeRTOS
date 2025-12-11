//==============================================
// TaskWiFiSetup.h
//==============================================

#ifndef TASK_WIFI_SETUP_H
#define TASK_WIFI_SETUP_H

#include <Arduino.h>
#include <WiFi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// WiFi status tracking
extern bool wifiConnected;
extern unsigned long lastWiFiCheck;

// WiFi setup functions
void initWiFi(const char *ssid, const char *password);
bool isWiFiConnected();
void taskWiFiMonitor(void *pv);

#endif