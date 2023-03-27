#include "HR4988.h"

HR4988 :: HR4988 (
    uint8_t enable_pin, uint8_t sleep_pin, uint8_t reset_pin,
    uint8_t step_pin, uint8_t direction_pin,
    uint8_t ms1_pin, uint8_t ms2_pin, uint8_t ms3_pin,
    int steps_per_turn = 200, float deg_per_full_step = 1.8 )
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


void HR4988 :: setup() {

}