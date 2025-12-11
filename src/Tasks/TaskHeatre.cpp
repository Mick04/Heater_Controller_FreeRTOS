#include "Pins.h"
#include "Globals.h"
#include "Config.h"

void taskHeater(void *pv) {
    pinMode(RELAY_PIN, OUTPUT);

    for (;;) {
        xSemaphoreTake(dataMutex, portMAX_DELAY);
        float current = coolsideTemp;
        float target  = targetTemp;
        xSemaphoreGive(dataMutex);

        if (current < target - 0.3) {
            digitalWrite(RELAY_PIN, HIGH);
            heaterOn = true;
        } else if (current > target + 0.3) {
            digitalWrite(RELAY_PIN, LOW);
            heaterOn = false;
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}