#pragma once
#include <Arduino.h>

#include "HR4988.h"


#define FEEDBACKSTEPPER_DEBUG 1


#define FEEDBACKSTEPPER_LIMIT_SWITCH_BEGIN_TYPE 0
#define FEEDBACKSTEPPER_LIMIT_SWITCH_END_TYPE   1


class AS5600;
class Potentiometer;
#include "AS5600.h"
#include "Potentiometer.h"

#include "button.h"


class FeedbackStepper : public HR4988 {

    private:
        AS5600 *rotative_encoder;
        int8_t increase_encoder_direction_sign;

        Potentiometer *linear_potentiometer;
        int8_t increase_potentiometer_direction_sign;

        uint8_t *limit_begin_reached;
        button_parameters *switch_begin_paramters;
        uint8_t *limit_end_reached;
        button_parameters *switch_end_parameters;

        uint8_t dummy_limit_reached;

        int *gears;
        int *gears_lin;


    private:
        void _shift_overshoot(int delta_pos);
        void _shift_linear_correction(int next_gear);


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
        
        void set_rotative_encoder(AS5600 *rotative_encoder, int8_t increase_encoder_direction_sign);
        void set_linear_potentiometer(Potentiometer *linear_potentiometer, int8_t increase_potentiometer_direction_sign);
        void set_limit_switch_begin(uint8_t *limit_begin_reached, button_parameters *switch_begin_paramters);
        void set_limit_switch_end(uint8_t *limit_end_reached, button_parameters *switch_end_parameters);
        void set_gears(int *gears);
        void set_gears_lin(int *gears_lin);
        
        void shift(int next_gear);
        void move_while_button_pressed(float speed, uint8_t *button_pressed, button_parameters *bp);
        void go_to_limit_switch(uint8_t limit_switch_type);
};