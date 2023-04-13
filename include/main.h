#include <Arduino.h>

#include "HR4988.h"
#include "Button.h"

#define DELAY_WAIT_SEMAPHORE 10


#define ENABLE_PIN 18
#define SLEEP_PIN 21
#define RESET_PIN 19

#define STEP_PIN 12
#define DIRECTION_PIN 14

#define MS1_PIN 25
#define MS2_PIN 33
#define MS3_PIN 32

#define LIMIT_SWITCH_PIN 23


const int steps_per_turn = 200;
const float deg_per_full_step = 1.8;

float rpm;
int steps_per_activation;
int current_steps = 0;

uint8_t limit_reached = 0;

void IRAM_ATTR limit_switch_isr();

char message[100];
uint8_t print = 0;


HR4988 stepper_motor = HR4988 (
    ENABLE_PIN, SLEEP_PIN, RESET_PIN,
    STEP_PIN, DIRECTION_PIN,
    MS1_PIN, MS2_PIN, MS3_PIN,
    steps_per_turn, deg_per_full_step
);
button_parameters limit_switch_parameters = {
    LIMIT_SWITCH_PIN, INPUT_PULLUP, LOW, limit_switch_isr, FALLING
};


SemaphoreHandle_t semaphore;

TaskHandle_t task_core_1;
TaskHandle_t task_core_0;

void function_core_1 (void *parameters);
void function_core_0 (void *parameters);