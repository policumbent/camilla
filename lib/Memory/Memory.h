#pragma once
#include <Arduino.h>
#include <Preferences.h>


class Memory {
    
    private:
        SemaphoreHandle_t mutex;
        Preferences preferences;

    public:
        Memory();

        void write_array(const char *key, void *array, int array_size, int element_size);
        void read_array(const char *key, void *array, int array_size, int element_size);
};