#pragma once
#include <Arduino.h>


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

        
        
}