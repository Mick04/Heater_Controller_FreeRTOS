#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "Globals.h"
#include "Config.h"
#include "TaskWiFiSetup.h"

WiFiClientSecure wifi;
PubSubClient mqtt(wifi);

void reconnectMQTT()
{
    Serial.println("❗️❗️❗️❗️❗️❗️❗️❗️❗️❗️❗️❗️❗️❗️❗️❗️");
    Serial.println("Attempting MQTT connection...");
    Serial.printf("MQTT Broker: %s:%d\n", MQTT_HOST, MQTT_PORT);
    Serial.println("❗️❗️❗️❗️❗️❗️❗️❗️❗️❗️❗️❗️❗️❗️❗️❗️");

    while (!mqtt.connected())
    {
        Serial.print("Connecting to MQTT...");
        if (mqtt.connect("HeaterController", MQTT_USER, MQTT_PASS))
        {
            Serial.println(" SUCCESS!");
            Serial.println("✅ MQTT Connected!");
            mqtt.subscribe("targetTemp");
            Serial.println("📡 Subscribed to 'targetTemp' topic");
            break;
        }
        else
        {
            Serial.print(" FAILED! Error code: ");
            Serial.println(mqtt.state());
            Serial.println("Retrying in 5 seconds...");
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
    }
}

void taskMQTT(void *pv)
{
    Serial.println("Starting MQTT Task");
    Serial.printf("");
    // Wait for WiFi to be connected before starting MQTT
    while (!isWiFiConnected())
    {
        Serial.println("Waiting for WiFi connection...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Configure TLS connection (skip certificate verification for simplicity)
    wifi.setInsecure();

    mqtt.setServer(MQTT_HOST, MQTT_PORT);

    for (;;)
    {
        // Only try MQTT if WiFi is connected
        if (isWiFiConnected())
        {
            if (!mqtt.connected())
                reconnectMQTT();
            mqtt.loop();

            xSemaphoreTake(dataMutex, portMAX_DELAY);
            mqtt.publish("outside", String(outsideTemp).c_str());
            mqtt.publish("coolside", String(coolsideTemp).c_str());
            mqtt.publish("heater", String(heaterTemp).c_str());
            xSemaphoreGive(dataMutex);
        }
        else
        {
            Serial.println("WiFi not connected, skipping MQTT operations");
        }

        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}