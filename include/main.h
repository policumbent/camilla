#pragma once
#include <Arduino.h>

#include "debug.h"

#include "FeedbackStepper.h"
#include "HR4988.h"
#include "AS5600.h"
#include "Potentiometer.h"
#include "button.h"
#include "Memory.h"


#define DELAY_WAIT_SEMAPHORE 10


extern SemaphoreHandle_t g_semaphore;     // xSemaphoreTake, xSemaphoreGive

void function_core_1 (void *parameters);
void function_core_0 (void *parameters);

extern uint8_t g_current_gear;

void gears_calibration();