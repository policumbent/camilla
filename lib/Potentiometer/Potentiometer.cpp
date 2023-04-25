#include "Potentiometer.h"


Potentiometer :: Potentiometer(uint8_t analog_pin) {
    this->analog_pin = analog_pin;

    setup();
}


void Potentiometer :: setup() {
    pinMode(analog_pin, INPUT);
}


uint16_t Potentiometer :: read_position() {
    position = analogRead(analog_pin);
    return position;
}


uint16_t Potentiometer :: get_position() {
    return position;
}


void Potentiometer :: calibration(HR4988 stepper_motor) {

}