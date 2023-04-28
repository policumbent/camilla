#pragma once
#include <Arduino.h>

#include "HR4988.h"
class HR4988;
#include "Potentiometer.h"      // not used, but otherwise it does not compile


class AS5600 {
    
    private:
        uint8_t analog_pin;
        uint16_t angle;

    public:
        AS5600 (uint8_t analog_pin);
        void setup();

        uint16_t read_angle();
        uint16_t get_angle();


        void calibration(HR4988 &stepper_motor);
};