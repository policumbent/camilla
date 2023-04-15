#include "Memory.h"


Memory :: Memory() {
    mutex = xSemaphoreCreateBinary();       // binary not mutex, since does not block scheduler
    xSemaphoreGive(mutex);
}


void Memory :: write_gears(int *gears, int size) {
    xSemaphoreTake(mutex, portMAX_DELAY);       // wait forever

    preferences.begin(gears_key, false);
    preferences.putBytes(gears_key, (void *) gears, size * sizeof(int));
    preferences.end();
    
    xSemaphoreGive(mutex);
}


void Memory :: read_gears(int *gears, int size) {
    xSemaphoreTake(mutex, portMAX_DELAY);       // wait forever

    preferences.begin(gears_key, false);
    if (preferences.isKey(gears_key)) {
        preferences.getBytes(gears_key, (void *) gears, size * sizeof(int));
    }
    preferences.end();

    xSemaphoreGive(mutex);
}