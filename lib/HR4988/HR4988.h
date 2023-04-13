#pragma once
#include <Arduino.h>

#define RPM_TO_DELAY_OFF(rpm) ((int) ((((float) deg_per_full_step / (float) microstepping) * 60.0e6) / (360.0 * rpm) - (float) delay_on))

#define FULL_STEP_MODE      1
#define HALF_STEP_MODE      2
#define QUARTER_STEP_MODE   4
#define EIGHT_STEP_MODE     8
#define SIXTEENTH_STEP_MODE 16

#define CW  0   // TODO: to be checked
#define CCW 1   // TODO: to be checked


class HR4988 {
    
    private:
        uint8_t enable_pin;     // negated pin
        uint8_t sleep_pin;      // negated pin
        uint8_t reset_pin;      // negated pin

        uint8_t step_pin;
        uint8_t direction_pin;

        uint8_t ms1_pin;
        uint8_t ms2_pin;
        uint8_t ms3_pin;

    private:
        uint8_t steps_per_turn;
        float deg_per_full_step;

        uint8_t enable;
        uint8_t reset;
        uint8_t _sleep;
        uint8_t _on;

        const uint8_t delay_on = 1;
        int delay_off;

        uint8_t direction;
        uint8_t microstepping;
        float rpm;
        int position;

    public:
        HR4988 (uint8_t enable_pin, uint8_t sleep_pin, uint8_t reset_pin,
                uint8_t step_pin, uint8_t direction_pin,
                uint8_t ms1_pin, uint8_t ms2_pin, uint8_t ms3_pin,
                int steps_per_turn = 200, float deg_per_full_step = 1.8);
        void setup();
        
        void set_speed (float speed);
        void change_direction ();
        void set_direction (uint8_t dir);
        void set_microstepping (uint8_t mode);
        void step();

        uint8_t get_direction();
        float get_speed();
        uint8_t get_microstepping();

        void on();
        void off();
        void sleep();
        void awake();

        int is_on();
        int is_sleep();

        void print_status();
};