#include <DallasTemperature.h>
#include "Globals.h"
#include "Pins.h"

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void taskReadSensors(void *pv)
{
    sensors.begin();

    for (;;)
    {
        sensors.requestTemperatures();
        float t1 = sensors.getTempCByIndex(0);
        float t2 = sensors.getTempCByIndex(1);
        float t3 = sensors.getTempCByIndex(2);

        xSemaphoreTake(dataMutex, portMAX_DELAY);
        heaterTemp = t1;
        coolsideTemp = t3;
        outsideTemp = t2;
        xSemaphoreGive(dataMutex);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}