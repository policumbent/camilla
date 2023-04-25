#pragma once
#include <Arduino.h>

#include "HR4988.h"
class HR4988;


class AS5600 {
    
    private:
        uint8_t analog_pin;
        uint16_t raw;
        float angle;

    public:
        AS5600 (uint8_t analog_pin);
        void setup();

        uint16_t read_raw();
        uint16_t get_raw();

        float read_angle();
        float get_angle();


        void calibration(HR4988 &stepper_motor);
};