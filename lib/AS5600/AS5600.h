#pragma once
#include <Arduino.h>
#include <driver/i2c.h>
#include <driver/adc.h>


// Circular reference solution
class HR4988;
class Potentiometer;

#include "HR4988.h"
#include "Potentiometer.h"


class AS5600 {
    
    private:
        uint8_t analog_pin;
        int angle;

    public:
        AS5600 (uint8_t analog_pin);
        void setup();

        int read_angle();
        int get_angle();

        uint16_t read_angle_output();

        void calibration(HR4988 &stepper_motor);
};