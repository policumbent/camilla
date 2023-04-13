#include <Arduino.h>

#define DELAY_WAIT_SEMAPHORE 10

char message[100];
uint8_t print = 0;


SemaphoreHandle_t semaphore;

TaskHandle_t task_core_1;
TaskHandle_t task_core_0;

void function_core_1 (void *parameters);
void function_core_0 (void *parameters);