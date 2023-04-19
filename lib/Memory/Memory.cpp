#include "Memory.h"


Memory :: Memory() {
    mutex = xSemaphoreCreateBinary();       // binary not mutex, since does not block scheduler
    xSemaphoreGive(mutex);
}


void Memory :: write_array(const char *key, void *array, int array_size, int element_size) {
    xSemaphoreTake(mutex, portMAX_DELAY);       // wait forever

    preferences.begin(key, false);
    preferences.putBytes(key, array, array_size * element_size);
    preferences.end();
    
    xSemaphoreGive(mutex);
}


void Memory :: read_array(const char *key, void *array, int array_size, int element_size) {
    xSemaphoreTake(mutex, portMAX_DELAY);       // wait forever

    preferences.begin(key, false);
    if (preferences.isKey(key)) {
        preferences.getBytes(key, array, array_size * element_size);
    }
    preferences.end();

    xSemaphoreGive(mutex);
}