#include "main.h"


void function_core_0 (void *parameters) {

#if DEBUG_CORES
    Serial.print("Task 0 initialized running on core: ");
    Serial.println(xPortGetCoreID());
#endif

    while (1) {
    
        if (print && xSemaphoreTake(semaphore, portMAX_DELAY)) {
            Serial.print(message);
            print = 0;
            xSemaphoreGive(semaphore);
        }
        delay(100);

    }
}