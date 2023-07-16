#include "Potentiometer.h"


Potentiometer :: Potentiometer(adc1_channel_t adc_channel) {
    this->adc_channel = adc_channel;

    setup();
}


void Potentiometer :: setup() {
    adc1_config_width(ADC_WIDTH_BIT_12);
}


uint16_t Potentiometer :: read_position() {
    const uint8_t ARRAY_SIZE = 3;
    uint16_t readings[ARRAY_SIZE], avg;
    uint16_t biggest_distance, biggest_distance_index, current_distance;
    int i;

    avg = 0;
    for (i=0; i<ARRAY_SIZE; i++) {
        readings[i] = adc1_get_raw(adc_channel);
        avg += readings[i];
    }
    avg = avg / ARRAY_SIZE;

    biggest_distance = abs(readings[0] - avg);
    biggest_distance_index = 0;
    for (i=1; i<ARRAY_SIZE; i++) {
        current_distance = abs(readings[i] - avg);
        if (current_distance > biggest_distance) {
            biggest_distance = current_distance;
            biggest_distance_index = i;
        }
    }

    avg = 0;
    for (i=0; i<ARRAY_SIZE; i++) {
        if (i != biggest_distance_index) {
            avg += readings[i];
        }
    }
    avg = avg / (ARRAY_SIZE - 1);

    position = avg;
    return position;
}


uint16_t Potentiometer :: get_position() {
    return position;
}


void Potentiometer :: calibration(HR4988 &stepper_motor) {
    Serial.println("\nPotentiometer calibration");
}