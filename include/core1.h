#include "main.h"

#define NUM_GEARS  12     // for Phoenix


#define ENABLE_PIN       18

#define STEP_PIN         12
#define DIRECTION_PIN    14

// microstepping pins
#define MS1_PIN          25
#define MS2_PIN          33
#define MS3_PIN          32


#define LIMIT_SWITCH_PIN 23

#define SHIFT_UP_BUTTON_PIN   16
#define SHIFT_DOWN_BUTTON_PIN 17


#define MAGNETIC_ENCODER_PIN 27

#define POTENTIOMETER_PIN 35        // TODO: not connected at the moment


const int full_steps_per_turn = 200;
const float deg_per_full_step = 1.8;
const int8_t cw_direction_sign = -1;

const char gears_memory_key[] = "gears_key";


extern HR4988 stepper_motor;
extern AS5600 rotative_encoder;
extern Potentiometer linear_potentiometer;
extern Memory flash;
extern int gears[];
extern uint8_t limit_reached;
extern uint8_t shift_up_pressed;
extern uint8_t shift_down_pressed;


void shift(uint8_t next_gear);

void test_mode();


void calibration();