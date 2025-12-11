#include <WiFi.h>
#include <PubSubClient.h>
#include "Globals.h"
#include "Config.h"

WiFiClient wifi;
PubSubClient mqtt(wifi);

void reconnectMQTT() {
    while (!mqtt.connected()) {
        if (mqtt.connect("HeaterController", MQTT_USER, MQTT_PASS)) {
            mqtt.subscribe("targetTemp");
        } else {
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
}

void taskMQTT(void *pv) {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    mqtt.setServer(MQTT_HOST, MQTT_PORT);

    for (;;) {
        if (!mqtt.connected()) reconnectMQTT();
        mqtt.loop();

        xSemaphoreTake(dataMutex, portMAX_DELAY);
        mqtt.publish("outside", String(outsideTemp).c_str());
        mqtt.publish("coolside", String(coolsideTemp).c_str());
        mqtt.publish("heater", String(heaterTemp).c_str());
        xSemaphoreGive(dataMutex);

        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}