#include "core0.h"


void function_core_0 (void *parameters) {

#if DEBUG_CORES
    Serial.print("Task 0 initialized running on core: ");
    Serial.println(xPortGetCoreID());
#endif

    delay(1000);

    while (1) {
    
        delay(10000);

    }
}