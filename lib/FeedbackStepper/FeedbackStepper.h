#pragma once
#include <Arduino.h>

#include "HR4988.h"


#define DEBUG_FEEDBACK_STEPPER 2


class AS5600;
class Potentiometer;
#include "AS5600.h"
#include "Potentiometer.h"


class FeedbackStepper : public HR4988 {

    private:
        AS5600 *rotative_encoder;
        Potentiometer *linear_potentiometer;
        uint8_t *limit_reached;


    public:
        FeedbackStepper (uint8_t step_pin, uint8_t direction_pin,
                         uint8_t ms1_pin, uint8_t ms2_pin, uint8_t ms3_pin,
                         uint8_t enable_pin, uint8_t sleep_pin, uint8_t reset_pin,
                         int full_steps_per_turn, float deg_per_full_step,
                         int8_t cw_direction_sign);

        FeedbackStepper (uint8_t step_pin, uint8_t direction_pin,
                         uint8_t ms1_pin, uint8_t ms2_pin, uint8_t ms3_pin,
                         uint8_t enable_pin,
                         int full_steps_per_turn, float deg_per_full_step,
                         int8_t cw_direction_sign);

        FeedbackStepper (uint8_t step_pin, uint8_t direction_pin,
                         uint8_t enable_pin,
                         int full_steps_per_turn, float deg_per_full_step,
                         int8_t cw_direction_sign);

        void setup();

        void set_rotative_encoder(AS5600 *rotative_encoder);
        void set_linear_potentiometer(Potentiometer *linear_potentiometer);
        void set_limit_switch(uint8_t *limit_reached);

        void move(int target_pos);
};