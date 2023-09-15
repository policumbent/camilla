#pragma once
#include <Arduino.h>

#include "settings.h"

#include "FeedbackStepper.h"
#include "HR4988.h"
#include "AS5600.h"
#include "Potentiometer.h"
#include "button.h"
#include "Memory.h"


#define LIMIT_SWITCH_END_PIN    18
#define LIMIT_SWITCH_BEGIN_PIN   5

#define SHIFT_UP_BUTTON_PIN     16        // green
#define SHIFT_DOWN_BUTTON_PIN   17        // yellow

#define CALIBRATION_BUTTON_PIN 4


extern TaskHandle_t task_core_1;
extern TaskHandle_t task_core_0;
extern TaskHandle_t task_webserver_calibration;


extern SemaphoreHandle_t g_semaphore;

void function_core_1 (void *parameters);
void function_core_0 (void *parameters);

extern uint8_t g_current_gear;
extern uint8_t g_calibration_flag;

void calibration();

void blink_built_in_led(uint8_t n_times);


extern uint8_t switch_begin_pressed;
extern uint8_t switch_end_pressed;
extern uint8_t shift_up_pressed;
extern uint8_t shift_down_pressed;
extern uint8_t calibration_button_pressed;
extern button_parameters limit_switch_begin_parameters;
extern button_parameters limit_switch_end_parameters;
extern button_parameters shift_up_button_parameters;
extern button_parameters shift_down_button_parameters;
extern button_parameters calibration_button_parameters;