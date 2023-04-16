#pragma once
#include <Arduino.h>

#define RPM_TO_DELAY_OFF(rpm) ((int) ((((float) deg_per_full_step / (float) microstepping) * 60.0e6) / (360.0 * rpm) - (float) delay_on))

#define FULL_STEP_MODE      1
#define HALF_STEP_MODE      2
#define QUARTER_STEP_MODE   4
#define EIGHT_STEP_MODE     8
#define SIXTEENTH_STEP_MODE 16

#define POSITION_CHANGE_FULL_MODE      16
#define POSITION_CHANGE_HALF_MODE      8
#define POSITION_CHANGE_QUARTER_MODE   4
#define POSITION_CHANGE_EIGHT_MODE     2
#define POSITION_CHANGE_SIXTEENTH_MODE 1

// Rotation seeing the rotating pole from up
#define CW  0
#define CCW 1

#define DELAY_CHANGE_DIRECTION 1e4

#define MAX_RPM 500


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

        const uint8_t delay_on = 1;
        int delay_off;

        uint8_t direction;
        uint8_t microstepping;
        uint8_t position_change;
        float rpm;
        int position_sixteenth;

    public:
        HR4988 (uint8_t enable_pin, uint8_t sleep_pin, uint8_t reset_pin,
                uint8_t step_pin, uint8_t direction_pin,
                uint8_t ms1_pin, uint8_t ms2_pin, uint8_t ms3_pin,
                int steps_per_turn = 200, float deg_per_full_step = 1.8);
        void setup();
        
        void move(int start_pos, int target_pos);
        void step();

        void set_position(int position);
        int get_position();

        void set_speed (float speed);
        float get_speed();

        void change_direction ();
        void set_direction (uint8_t dir);
        uint8_t get_direction();

        void set_microstepping (uint8_t mode);
        uint8_t get_microstepping();

        void sleep();
        void awake();

        int is_sleep();

        void debug_serial_control();
        void print_status();
};