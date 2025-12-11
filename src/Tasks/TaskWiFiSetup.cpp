#include "TaskWiFiSetup.h"
#include "Config.h"
#include "Globals.h"

// WiFi status tracking
bool wifiConnected = false;
unsigned long lastWiFiCheck = 0;

// WiFi event callback
void WiFiEvent(WiFiEvent_t event) {
    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.println("WiFi connected!");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
            wifiConnected = true;
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("WiFi disconnected!");
            wifiConnected = false;
            break;
        default:
            break;
    }
}

void initWiFi(const char* ssid, const char* password) {
    // Set WiFi event handler
    WiFi.onEvent(WiFiEvent);
    
    // Configure WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    
    Serial.printf("Connecting to WiFi: %s\n", ssid);
    
    // Wait for initial connection (up to 30 seconds)
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 60) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        Serial.println("\nWiFi connected successfully!");
        Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
    } else {
        Serial.println("\nFailed to connect to WiFi!");
        wifiConnected = false;
    }
}

bool isWiFiConnected() {
    return wifiConnected && (WiFi.status() == WL_CONNECTED);
}

void taskWiFiMonitor(void *pv) {
    // Initial WiFi setup
    initWiFi(WIFI_SSID, WIFI_PASS);
    
    for (;;) {
        // Check WiFi status every 10 seconds
        if (millis() - lastWiFiCheck > 10000) {
            lastWiFiCheck = millis();
            
            if (WiFi.status() != WL_CONNECTED) {
                if (wifiConnected) {
                    Serial.println("WiFi connection lost! Attempting to reconnect...");
                    wifiConnected = false;
                }
                
                // Try to reconnect
                WiFi.disconnect();
                vTaskDelay(pdMS_TO_TICKS(1000));
                WiFi.begin(WIFI_SSID, WIFI_PASS);
                
                // Wait up to 10 seconds for reconnection
                int attempts = 0;
                while (WiFi.status() != WL_CONNECTED && attempts < 20) {
                    vTaskDelay(pdMS_TO_TICKS(500));
                    attempts++;
                }
                
                if (WiFi.status() == WL_CONNECTED) {
                    Serial.println("WiFi reconnected successfully!");
                    wifiConnected = true;
                } else {
                    Serial.println("WiFi reconnection failed!");
                }
            } else {
                // WiFi is connected, update status if needed
                if (!wifiConnected) {
                    wifiConnected = true;
                    Serial.println("WiFi status restored");
                }
            }
        }
        
        // Sleep for 5 seconds between checks
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
