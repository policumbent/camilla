#pragma once
#include "main.h"


#define LIMIT_SWITCH_PIN 23

#define SHIFT_UP_BUTTON_PIN   16
#define SHIFT_DOWN_BUTTON_PIN 17


extern uint8_t limit_reached;
extern uint8_t shift_up_pressed;
extern uint8_t shift_down_pressed;


void shift(uint8_t next_gear);


void calibration();