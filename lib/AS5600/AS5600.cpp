#include "AS5600.h"


AS5600 :: AS5600 (uint8_t analog_pin) {
    this->analog_pin = analog_pin;

    setup();
}


void AS5600 :: setup() {
    pinMode(analog_pin, INPUT);
}


uint16_t AS5600 :: read_angle() {
    angle = analogRead(analog_pin);
    return angle;
}


uint16_t AS5600 :: get_angle() {
    return angle;
}


void AS5600 :: calibration(HR4988 &stepper_motor) {

    Serial.println("\nEncoder calibration");
    
    pinMode(analog_pin, OUTPUT);
    digitalWrite(analog_pin, LOW);
    delay(120);
    pinMode(analog_pin, INPUT);

    stepper_motor.set_direction(CW);
    stepper_motor.set_speed(60);

    int start_pos = stepper_motor.get_position();
    while (stepper_motor.get_position() < start_pos + stepper_motor.get_delta_position_360_degrees_rotation()) {
        stepper_motor.step();
    }

    pinMode(analog_pin, OUTPUT);
    digitalWrite(analog_pin, LOW);
    delay(120);
    pinMode(analog_pin, INPUT);
}