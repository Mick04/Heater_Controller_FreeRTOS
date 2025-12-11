#ifndef CONFIG_H
#define CONFIG_H

// WiFi
extern const char* WIFI_SSID;
extern const char* WIFI_PASS;

// MQTT
extern const char* MQTT_HOST;
extern const int   MQTT_PORT;
extern const char* MQTT_USER;
extern const char* MQTT_PASS;

// Temperature Setpoint
extern float targetTemp;

#endif