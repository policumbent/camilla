#include "HR4988.h"

HR4988 :: HR4988 (
    uint8_t enable_pin, uint8_t sleep_pin, uint8_t reset_pin,
    uint8_t step_pin, uint8_t direction_pin,
    uint8_t ms1_pin, uint8_t ms2_pin, uint8_t ms3_pin,
    int steps_per_turn, float deg_per_full_step )
{
    this->enable_pin = enable_pin;
    this->sleep_pin = sleep_pin;
    this->reset_pin = reset_pin;
    this->step_pin = step_pin;
    this->direction_pin = direction_pin;
    this->ms1_pin = ms1_pin;
    this->ms2_pin = ms2_pin;
    this->ms3_pin = ms3_pin;
    this->steps_per_turn = steps_per_turn;
    this->deg_per_full_step = deg_per_full_step;

    setup();
}


void HR4988 :: setup () {
    pinMode(enable_pin, OUTPUT);
    pinMode(reset_pin, OUTPUT);
    pinMode(sleep_pin, OUTPUT);
    
    pinMode(step_pin, OUTPUT);
    pinMode(direction_pin, OUTPUT);

    pinMode(ms1_pin, OUTPUT);
    pinMode(ms2_pin, OUTPUT);
    pinMode(ms3_pin, OUTPUT);

    enable = 1;
    _sleep = 0;
    reset = 0;
    _on = 1;
    digitalWrite(enable_pin, !enable);
    digitalWrite(reset_pin, !reset);
    digitalWrite(sleep_pin, !_sleep);

    direction = 0;
    microstepping = FULL_STEP_MODE;
    rpm = 120.0;
    change_direction(direction);
    set_microstepping(microstepping);
    set_speed(rpm);
}


void HR4988 :: set_speed (float speed) {
    int new_delay;

    rpm = speed;
    new_delay = RPM_TO_DELAY_OFF(rpm);
    new_delay = (new_delay < 1000000) ? (new_delay):(1000000);
    delay_off = new_delay;
}


void HR4988 :: change_direction () {
    direction = 1 - direction;
    digitalWrite(direction_pin, direction);
    delayMicroseconds(10000);
}


void HR4988 :: change_direction (int dir) {
    if (direction == dir)
        return;
    
    change_direction();
}


void HR4988 :: set_microstepping (int mode) {
    uint8_t ms1, ms2, ms3;

    if (microstepping == mode)
        return;

    switch (mode) {
        case FULL_STEP_MODE:
            ms1 = 0; ms2 = 0; ms3 = 0; break;
        case HALF_STEP_MODE:
            ms1 = 1; ms2 = 0; ms3 = 0; break;
        case QUARTER_STEP_MODE:
            ms1 = 0; ms2 = 1; ms3 = 0; break;
        case EIGHT_STEP_MODE:
            ms1 = 1; ms2 = 1; ms3 = 0; break;
        case SIXTEENTH_STEP_MODE:
            ms1 = 1; ms2 = 1; ms3 = 1; break;
        default: break;
    }

    digitalWrite(ms1_pin, ms1);
    digitalWrite(ms2_pin, ms2);
    digitalWrite(ms3_pin, ms3);
    
    microstepping = mode;
    set_speed(rpm);
}


void HR4988 :: step () {
    if (!_on)
        return;

    digitalWrite(step_pin, HIGH);
    delayMicroseconds(delay_on);
    digitalWrite(step_pin, LOW);
    delayMicroseconds(delay_off);
}


void HR4988 :: on() {
    _on = 1;
}


void HR4988 :: off() {
    _on = 0;
}


void HR4988 :: sleep() {
    _sleep = 1;
    digitalWrite(sleep_pin, !_sleep);
}


void HR4988 :: awake() {
    _sleep = 0;
    digitalWrite(sleep_pin, !_sleep);
}


int HR4988 :: is_on() {
    return _on;
}


int HR4988 :: is_sleep() {
    return _sleep;
}


void HR4988 :: print_status() {
    if (!Serial)
        return;
        
    char str[100];
    sprintf(str, "RPM: %.2f\tDirection: %d\tMicrostepping: %d\tSleep: %d\tOn: %d\tDelay off: %d",
            rpm, direction, microstepping, _sleep, _on, delay_off);
    Serial.println(str);
}