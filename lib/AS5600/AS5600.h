#pragma once
#include <Arduino.h>
#include <driver/i2c.h>
#include <driver/adc.h>


// Circular reference solution
class HR4988;
class Potentiometer;

#include "HR4988.h"
#include "Potentiometer.h"


#define I2C_ESP 1
#define WIRE_H  0


#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_FREQ_HZ 1000000

#define AS5600_I2C_ADDRESS 0x36
#define AS5600_I2C_LOW_BYTE_CMD 0x0D//D//F
#define AS5600_I2C_HIGH_BYTE_CMD 0x0C//C//E

#define AS5600_I2C_STATUS_BYTE_CMD 0x0B


class AS5600 {
    
    private:
        uint8_t analog_pin;
        int angle;

        int i2c_master_port;
        i2c_config_t i2c_conf;

    public:
        AS5600();
        AS5600 (uint8_t analog_pin);
        void setup();

        #if I2C_ESP
            int8_t get_magnet_distance();
        #endif

        int read_angle();
        int get_angle();

        uint16_t read_angle_output();

        void calibration(HR4988 &stepper_motor);
};