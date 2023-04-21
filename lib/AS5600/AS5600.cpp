#include "AS5600.h"


AS5600 :: AS5600 (uint8_t analog_pin) {
    this->analog_pin = analog_pin;

    setup();
}


void AS5600 :: setup() {
    pinMode(analog_pin, INPUT);
}


int AS5600 :: read_raw() {
    return analogRead(analog_pin);
}


void AS5600 :: calibration(HR4988 stepper_motor) {
    
    pinMode(analog_pin, OUTPUT);
    digitalWrite(analog_pin, LOW);
    delay(120);
    pinMode(analog_pin, INPUT);

    stepper_motor.set_direction(CW);
    stepper_motor.set_speed(60);

    int start_pos = stepper_motor.get_position();
    while (stepper_motor.get_position() < start_pos + stepper_motor.get_delta_position_turn()) {
        stepper_motor.step();
    }

    pinMode(analog_pin, OUTPUT);
    digitalWrite(analog_pin, LOW);
    delay(120);
    pinMode(analog_pin, INPUT);
}