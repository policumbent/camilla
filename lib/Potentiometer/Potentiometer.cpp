#include "Potentiometer.h"


Potentiometer :: Potentiometer(uint8_t analog_pin) {
    this->analog_pin = analog_pin;

    setup();
}


void Potentiometer :: setup() {
    pinMode(analog_pin, INPUT);
}


uint16_t Potentiometer :: read_raw() {
    raw = analogRead(analog_pin);
    return raw;
}


uint16_t Potentiometer :: get_raw() {
    return raw;
}


float Potentiometer :: read_position() {
    read_raw();
    position = (float) raw / 4095.0 * 55.0;     // TODO: check conversion
    return position;
}


float Potentiometer :: get_position() {
    return position;
}


void Potentiometer :: calibration(HR4988 stepper_motor) {

}