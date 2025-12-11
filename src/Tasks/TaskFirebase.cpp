#include <Firebase_ESP_Client.h>
#include "Globals.h"
#include "Config.h"
#include "FirebaseConfig.h"

// Firebase objects
FirebaseData fbData;
FirebaseAuth fbAuth;
FirebaseConfig fbConfig;

// Paths in your Realtime DB
String pathTargetTemp = "/targetTemp";
String pathOutside     = "/outsideTemp";
String pathCoolside    = "/coolsideTemp";
String pathHeater      = "/heaterTemp";

void taskFirebase(void *pv) {
    // Configure Firebase
    fbConfig.api_key = FIREBASE_AUTH;
    fbAuth.token.uid = "";  // not required for simple DB access
    Firebase.begin(&fbConfig, &fbAuth);
    Firebase.reconnectWiFi(true);

    for (;;) {
        // --- PUBLISH SENSOR DATA ---
        xSemaphoreTake(dataMutex, portMAX_DELAY);
        float outT = outsideTemp;
        float coolT = coolsideTemp;
        float heatT = heaterTemp;
        xSemaphoreGive(dataMutex);

        if (Firebase.RTDB.setFloat(&fbData, pathOutside.c_str(), outT)) {
            // Success
        }
        if (Firebase.RTDB.setFloat(&fbData, pathCoolside.c_str(), coolT)) {}
        if (Firebase.RTDB.setFloat(&fbData, pathHeater.c_str(), heatT)) {}

        // --- PULL TARGET TEMP ---
        if (Firebase.RTDB.getFloat(&fbData, pathTargetTemp.c_str())) {
            xSemaphoreTake(dataMutex, portMAX_DELAY);
            targetTemp = fbData.floatData();
            xSemaphoreGive(dataMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(3000)); // every 3 seconds
    }
}