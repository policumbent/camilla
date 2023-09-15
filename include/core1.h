#pragma once
#include "main.h"


#define NUM_GEARS  12     // for Phoenix

#define CALIBRATION_SPEED          200
#define LIMIT_SWITCH_PRESSED_SPEED 200

#define DISTANCE_FROM_LIMIT_SWITCHES 6400

#define TIME_BUTTON_LONG_PRESS 2000


// Stepper pins
#define ENABLE_PIN      13

#define MS1_PIN         12
#define MS2_PIN         14
#define MS3_PIN         27

#define RESET_PIN       26
#define SLEEP_PIN       25

#define STEP_PIN        33
#define DIRECTION_PIN   32


#define POTENTIOMETER_ADC_CHANNEL ADC1_CHANNEL_6


const int full_steps_per_turn = 200;
const float deg_per_full_step = 1.8;
const int8_t cw_direction_sign = -1;

const int8_t increase_encoder_direction_sign = -1;
const int8_t increase_potentiometer_direction_sign = 1;

const char gears_memory_key[] = "gears_key";
const char gears_lin_memory_key[] = "gears_lin_key";


extern FeedbackStepper stepper_motor;
extern AS5600 rotative_encoder;
extern Potentiometer linear_potentiometer;
extern Memory flash;

extern int gears[];
extern int gears_lin[];

extern uint8_t zero_reference_limit_switch_type;


void gears_mode();
void while_pressed_mode();

#if NEVADA_MODE
    void shift(uint8_t next_gear, int delta_manual);
#else
    void shift(uint8_t next_gear);
#endif

void move_while_pressed_buttons_control();
void initialize_position();