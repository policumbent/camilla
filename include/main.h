#pragma once
#include <Arduino.h>

#include "debug.h"

#include "HR4988.h"
#include "AS5600.h"
#include "Potentiometer.h"
#include "button.h"
#include "Memory.h"


#define DELAY_WAIT_SEMAPHORE 10


#define NUM_GEARS        12     // for Phoenix


#define ENABLE_PIN       18

#define STEP_PIN         12
#define DIRECTION_PIN    14

// microstepping pins
#define MS1_PIN          25
#define MS2_PIN          33
#define MS3_PIN          32


#define MAGNETIC_ENCODER_PIN 27

#define POTENTIOMETER_PIN 35        // TODO: not connected at the moment

const int full_steps_per_turn = 200;
const float deg_per_full_step = 1.8;
const int8_t cw_direction_sign = -1;

const char gears_memory_key[] = "gears_key";


extern SemaphoreHandle_t g_sem_move;     // xSemaphoreTake, xSemaphoreGive
extern SemaphoreHandle_t g_sem_pos;

void function_core_1 (void *parameters);
void function_core_0 (void *parameters);

extern uint8_t g_current_gear;


extern HR4988 stepper_motor;
extern AS5600 rotative_encoder;
extern Potentiometer linear_potentiometer;
extern Memory flash;
extern int gears[];


void calibration();