#pragma once
#include <Arduino.h>
#include "HR4988.h"

class Potentiometer {

    private:
        uint8_t pin;
        int read_value;
        int position;

    public:
        Potentiometer(uint8_t pin);
        void setup();
        void calibration(HR4988 stepper_motor);
};