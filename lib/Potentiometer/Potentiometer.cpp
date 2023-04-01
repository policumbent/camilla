#include "Potentiometer.h"


Potentiometer :: Potentiometer(uint8_t pin) {
    this->pin = pin;

    setup();
}


void Potentiometer :: setup() {
    pinMode(pin, INPUT);
}


void Potentiometer :: calibration(HR4988 stepper_motor) {
    char c, str[100];
    int read, prev_read;
    long int t_begin;

    uint8_t prev_dir = stepper_motor.get_direction();
    float prev_speed = stepper_motor.get_speed();
    uint8_t prev_microstepping = stepper_motor.get_microstepping();
    
    stepper_motor.set_direction(0);
    stepper_motor.set_microstepping(SIXTEENTH_STEP_MODE);
    stepper_motor.set_speed(10);
    stepper_motor.on();
    
    Serial.println("Press 's' to start (at the end 's' to stop)");
    c = 'x';
    while (c != 's') {
        if (Serial.available()) {
            c = Serial.read();
        }
    }

    t_begin = millis();
    prev_read = analogRead(pin);
    c = 'x';
    while (c != 's') {
        if (Serial.available()) {
            c = Serial.read();
        }

        read = analogRead(pin);
        stepper_motor.step();

        if (read != prev_read) {
            sprintf(str, "Voltage: %d\tTime: %ld", read, millis() - t_begin);
            Serial.println(str);
            prev_read = read;
        }
    }

    stepper_motor.set_direction(prev_dir);
    stepper_motor.set_speed(prev_dir);
    stepper_motor.set_microstepping(prev_microstepping);
    stepper_motor.on();
}