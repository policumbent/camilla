#pragma once
#include <Arduino.h>

#include "HR4988.h"


class AS5600 {
    
    private:
        uint8_t analog_pin;
        int reading;

    public:
        AS5600 (uint8_t analog_pin);

        void setup();

        int read_raw();

        void calibration(HR4988 stepper_motor);
};