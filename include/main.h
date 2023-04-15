#pragma once
#include <Arduino.h>

#include "debug.h"


#define DELAY_WAIT_SEMAPHORE 10


extern SemaphoreHandle_t semaphore;     // xSemaphoreTake, xSemaphoreGive

void function_core_1 (void *parameters);
void function_core_0 (void *parameters);

extern uint8_t current_gear;