#pragma once
#include <Arduino.h>

#include "HR4988.h"


class AS5600 {
    
    private:
        uint8_t analog_pin;
        int raw;
        float angle;

    private:
        void _read_raw();

    public:
        AS5600 (uint8_t analog_pin);

        void setup();

        float read_angle();
        float get_angle();


        void calibration(HR4988 stepper_motor);
};