#pragma once
#include <Arduino.h>

#define RPM_TO_DELAY_OFF(rpm) ((int) ((((float) deg_per_full_step / (float) microstepping) * 60.0e6) / (360.0 * rpm) - (float) delay_on))

#define FULL_STEP_MODE 1
#define HALF_STEP_MODE 2
#define QUARTER_STEP_MODE 4
#define EIGHT_STEP_MODE 8
#define SIXTEENTH_STEP_MODE 16


class HR4988 {
    
    private:
        uint8_t enable_pin;
        uint8_t sleep_pin;
        uint8_t reset_pin;

        uint8_t step_pin;
        uint8_t direction_pin;

        uint8_t ms1_pin;
        uint8_t ms2_pin;
        uint8_t ms3_pin;

    private:
        uint8_t steps_per_turn;
        float deg_per_full_step;

        uint8_t enable = 1;
        uint8_t reset = 0;
        uint8_t sleep_ = 0;
        uint8_t on = 0;

        const uint8_t delay_on = 1;
        int delay_off;

        int direction = HIGH;
        int microstepping = FULL_STEP_MODE;
        float rpm = 10;

    public:
        HR4988 (uint8_t enable_pin, uint8_t sleep_pin, uint8_t reset_pin,
                uint8_t step_pin, uint8_t direction_pin,
                uint8_t ms1_pin, uint8_t ms2_pin, uint8_t ms3_pin,
                int steps_per_turn = 200, float deg_per_full_step = 1.8);
        void setup();
        
};