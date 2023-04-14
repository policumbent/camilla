#pragma once
#include <Arduino.h>

#define DEBUG_CORES 1


#define DELAY_WAIT_SEMAPHORE 10


extern char message[100];
extern uint8_t print;

extern SemaphoreHandle_t semaphore;

void function_core_1 (void *parameters);
void function_core_0 (void *parameters);