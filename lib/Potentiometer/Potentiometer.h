#pragma once
#include <Arduino.h>

#include <Wire.h>
#include <driver/adc.h>


// Circular reference solution
class HR4988;
class AS5600;

#include "HR4988.h"
#include "AS5600.h"


class Potentiometer {

    private:
        adc1_channel_t adc_channel;
        uint16_t position;

    public:
        Potentiometer(adc1_channel_t adc_channel);
        void setup();

        uint16_t read_position();
        uint16_t get_position();

        uint16_t raw_read();

        void calibration(HR4988 &stepper_motor);
};