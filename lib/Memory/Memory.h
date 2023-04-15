#pragma once
#include <Arduino.h>
#include <Preferences.h>


class Memory {
    
    private:
        SemaphoreHandle_t mutex;
        Preferences preferences;

        const char gears_key[10] = "gears_key";     // 10 is the length of the string

    public:
        Memory();

        void write_gears(int *gears, int size);
        void read_gears(int *gears, int size);
};