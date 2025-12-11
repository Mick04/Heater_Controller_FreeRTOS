#include <Firebase_ESP_Client.h>
#include <time.h>
#include "Globals.h"
#include "Config.h"
#include "FirebaseConfig.h"

// Firebase objects
FirebaseData fbData;
FirebaseAuth fbAuth;
FirebaseConfig fbConfig;

// Paths in your Realtime DB
String pathTargetTemp = "FreeRTOS/ESP32/targetTemp";
String pathOutside = "FreeRTOS/ESP32/outsideTemp";
String pathCoolside = "FreeRTOS/ESP32/coolsideTemp";
String pathHeater = "FreeRTOS/ESP32/heaterTemp";

void taskFirebase(void *pv)
{
    // Configure time synchronization (required for Firebase authentication)
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    // Wait for time synchronization
    time_t nowSecs = time(nullptr);
    int timeoutSec = 0;
    while (nowSecs < 8 * 3600 * 24 && timeoutSec < 15)
    {
        delay(500);
        Serial.print(".");
        yield();
        nowSecs = time(nullptr);
        timeoutSec++;
    }

    if (nowSecs >= 8 * 3600 * 24)
    {
        struct tm timeinfo;
        gmtime_r(&nowSecs, &timeinfo);
    }

    // Configure Firebase - CORRECTED VERSION
    // Set Firebase configuration
    fbConfig.database_url = FIREBASE_HOST;
    fbConfig.api_key = FIREBASE_AUTH;

    // For Firebase v4.x, we need to use anonymous authentication
    fbAuth.user.email = "";
    fbAuth.user.password = "";

    // Alternative: Use service account for better reliability
    // fbAuth.token.uid = "esp32-controller";

    // Set timeout and retry settings
    fbConfig.timeout.serverResponse = 10 * 1000; // 10 seconds
    fbConfig.timeout.socketConnection = 10 * 1000;
    fbConfig.max_token_generation_retry = 5;

    Firebase.begin(&fbConfig, &fbAuth);
    Firebase.reconnectWiFi(true);

    // Enable Firebase debug (optional)
    Firebase.setSystemTime(time(nullptr));

    if (Firebase.signUp(&fbConfig, &fbAuth, "", ""))
    {
        Serial.println("✅ Anonymous sign-up successful!");
    }
    else
    {
        Serial.println("⚠️ Anonymous sign-up failed, trying direct database access...");
        Serial.println("Error: " + String(fbConfig.signer.signupError.message.c_str()));
    }

    // Wait a moment for authentication to process
    vTaskDelay(pdMS_TO_TICKS(2000));

    if (Firebase.RTDB.setString(&fbData, "/test", "ESP32 connection test"))
    {
        Serial.println("✅ Firebase test write successful! Firebase is working!");
    }
    else
    {
        Serial.println("❌ Firebase test write failed!");
        Serial.println("Error: " + fbData.errorReason());
    }

    // Wait for Firebase to be ready (up to 30 seconds)
    int timeout = 30000; // 30 seconds
    int elapsed = 0;
    while (!Firebase.ready() && elapsed < timeout)
    {
        if (elapsed % 5000 == 0)
        { // Print status every 5 seconds
            Serial.printf("⏳ Waiting for Firebase... (%ds/%ds)\n", elapsed / 1000, timeout / 1000);

            // Try another test write every 10 seconds
            if (elapsed % 10000 == 0 && elapsed > 0)
            {
                Serial.println("🧪 Testing Firebase write during wait...");
                if (Firebase.RTDB.setString(&fbData, "/test", String("Test at ") + String(elapsed / 1000) + "s"))
                {
                    Serial.println("✅ Firebase write works! Skipping wait...");
                    break; // Firebase is working, exit wait loop
                }
                else
                {
                    Serial.println("❌ Firebase write still failing: " + fbData.errorReason());
                }
            }
        }
        Serial.print(".");
        vTaskDelay(pdMS_TO_TICKS(1000));
        elapsed += 1000;
    }

    if (Firebase.ready())
    {
        Serial.println("\n✅ Firebase is now ready!");
        Serial.println("🔗 Firebase connection established successfully!");
    }
    else
    {
        Serial.println("\n❌ Firebase failed to initialize within 30 seconds");
        Serial.println("Check: WiFi connection, Firebase credentials, and time sync");
    }

    for (;;)
    {
        Serial.println("Firebase Task Loop");
        Serial.println("Firebase Task Loop");
        // --- PUBLISH SENSOR DATA ---
        xSemaphoreTake(dataMutex, portMAX_DELAY);
        float outT = outsideTemp;
        float coolT = coolsideTemp;
        float heatT = heaterTemp;
        xSemaphoreGive(dataMutex);

        Serial.printf("📊 Sensor values: OUT=%.2f COOL=%.2f HEAT=%.2f\n", outT, coolT, heatT);
        Serial.println("📤 Attempting to send to Firebase...");

        // Try Firebase operations directly (Firebase.ready() may be unreliable)
        bool firebaseWorking = false;

        if (Firebase.RTDB.setFloat(&fbData, pathOutside.c_str(), outT))
        {
            Serial.println("✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓");
            Serial.println("Uploaded outside temp to Firebase");
            Serial.println("✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓");
            firebaseWorking = true;
        }
        else
        {
            Serial.println("❌ Failed to upload outside temp to Firebase");
            Serial.println("Error: " + fbData.errorReason());
        }

        if (Firebase.RTDB.setFloat(&fbData, pathCoolside.c_str(), coolT))
        {
            Serial.println("✅ Uploaded coolside temp to Firebase");
            firebaseWorking = true;
        }
        else
        {
            Serial.println("❌ Failed to upload coolside temp to Firebase");
            Serial.println("Error: " + fbData.errorReason());
        }

        if (Firebase.RTDB.setFloat(&fbData, pathHeater.c_str(), heatT))
        {
            Serial.println("✅ Uploaded heater temp to Firebase");
            firebaseWorking = true;
        }
        else
        {
            Serial.println("❌ Failed to upload heater temp to Firebase");
            Serial.println("Error: " + fbData.errorReason());
        }

        if (firebaseWorking)
        {
            Serial.println("🔥 Firebase operations successful!");
        }
        else
        {
            Serial.println("❌ All Firebase operations failed!");
        }

        // --- PULL TARGET TEMP ---
        if (Firebase.RTDB.getFloat(&fbData, pathTargetTemp.c_str()))
        {
            xSemaphoreTake(dataMutex, portMAX_DELAY);
            targetTemp = fbData.floatData();
            xSemaphoreGive(dataMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(3000)); // every 3 seconds
    }
}