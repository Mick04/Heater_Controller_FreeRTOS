#include "Globals.h"

void taskHeartbeat(void *pv)
{
    for (;;)
    {
        Serial.printf("OUT: %.2f  COOL: %.2f  HEAT: %.2f  Heater:%d\n",
                      outsideTemp, coolsideTemp, heaterTemp, heaterOn);

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}