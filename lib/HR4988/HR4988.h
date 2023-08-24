#pragma once
#include <Arduino.h>


#define HR4988_DEBUG 1


#define HR4988_FULL_STEP_MODE      1
#define HR4988_HALF_STEP_MODE      2
#define HR4988_QUARTER_STEP_MODE   4
#define HR4988_EIGHT_STEP_MODE     8
#define HR4988_SIXTEENTH_STEP_MODE 16

#define HR4988_POSITION_CHANGE_FULL_MODE      16
#define HR4988_POSITION_CHANGE_HALF_MODE      8
#define HR4988_POSITION_CHANGE_QUARTER_MODE   4
#define HR4988_POSITION_CHANGE_EIGHT_MODE     2
#define HR4988_POSITION_CHANGE_SIXTEENTH_MODE 1

#define HR4988_SIXTEENTH_MAX_RPM    120
#define HR4988_EIGHT_MODE_MAX_RPM   180
#define HR4988_QUARTER_MODE_MAX_RPM 240
#define HR4988_HALF_MODE_MAX_RPM    300

#define HR4988_MIN_MOVE_RPM 100
#define HR4988_MAX_RPM      600

#define HR4988_MAX_ACCELERATION_STEPS 800      // 1/4 turn

// Rotation seeing the rotating pole from up
#define HR4988_CW  0
#define HR4988_CCW 1

#define HR4988_POSITIVE_DIR     1
#define HR4988_NEGATIVE_DIR     -1
#define HR4988_CHANGE_DIR       0

#define HR4988_DELAY_CHANGE_DIRECTION 1e4


class HR4988 {
    
    private:
        uint8_t step_pin;
        uint8_t direction_pin;

        uint8_t ms1_pin;
        uint8_t ms2_pin;
        uint8_t ms3_pin;

        uint8_t enable_pin;     // negated pin
        uint8_t sleep_pin;      // negated pin
        uint8_t reset_pin;      // negated pin

    protected:
        uint8_t full_steps_per_turn;
        float deg_per_full_step;

        uint8_t enable;
        uint8_t reset;
        uint8_t _sleep;

        const uint8_t delay_on = 10;
        int delay_off;

        int8_t direction;
        uint8_t microstepping;
        uint8_t microstepping_on;
        uint8_t position_change;
        float rpm;
        int position_sixteenth;
        int8_t cw_direction_sign;


    protected:
        void _move_set_speed_direction(int start_pos, int target_pos);
        int _update_position();
        void _step_no_delay_off();
        void _set_delay_off();


    public:
        HR4988 (uint8_t step_pin, uint8_t direction_pin,
                uint8_t ms1_pin, uint8_t ms2_pin, uint8_t ms3_pin,
                uint8_t enable_pin, uint8_t sleep_pin, uint8_t reset_pin,
                int full_steps_per_turn, float deg_per_full_step,
                int8_t cw_direction_sign);

        HR4988 (uint8_t step_pin, uint8_t direction_pin,
                uint8_t ms1_pin, uint8_t ms2_pin, uint8_t ms3_pin,
                uint8_t enable_pin,
                int full_steps_per_turn, float deg_per_full_step,
                int8_t cw_direction_sign);

        HR4988 (uint8_t step_pin, uint8_t direction_pin,
                uint8_t enable_pin,
                int full_steps_per_turn, float deg_per_full_step,
                int8_t cw_direction_sign);

        void setup();
        
        void move_const_speed(int target_pos, float speed);
        void move(int target_pos);
        void step();

        void set_position(int position);
        int get_position();

        void set_speed(float speed);
        float get_speed();

        void change_direction ();
        void set_direction(int8_t dir);
        uint8_t get_direction();

        void set_microstepping(uint8_t mode);
        uint8_t get_microstepping();
        void disable_microstepping();
        void enable_microstepping();

        int get_delta_position_360_degrees_rotation();
        int get_expected_step_time();
        int8_t get_cw_direction_sign();

        void debug_serial_control();
        void print_status();

        void driver_calibration();
};