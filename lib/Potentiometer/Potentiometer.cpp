#include "Potentiometer.h"


Potentiometer :: Potentiometer(adc1_channel_t adc_channel) {
    this->adc_channel = adc_channel;

    setup();
}


void Potentiometer :: setup() {
    adc1_config_width(ADC_WIDTH_BIT_12);
}


uint16_t Potentiometer :: read_position() {
    position = adc1_get_raw(adc_channel);
    return position;
}


uint16_t Potentiometer :: get_position() {
    return position;
}


void Potentiometer :: calibration(HR4988 &stepper_motor) {
    Serial.println("\nPotentiometer calibration");
}