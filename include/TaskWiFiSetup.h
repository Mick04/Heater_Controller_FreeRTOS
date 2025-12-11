//==============================================
// WifiModule.h
//==============================================

#ifndef WIFI_MODULE_H
#define WIFI_MODULE_H

#include <Arduino.h>
#include <WiFi.h>

void initWiFi(const char* ssid, const char* password);
bool isWiFiConnected();
void taskWiFiMonitor(void *pv);

#endif