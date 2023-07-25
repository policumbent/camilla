#pragma once
#include <Arduino.h>

#define BUTTON_DEBOUNCING_TIME (int)50

struct button_parameters {
    uint8_t pin;
    uint8_t pin_mode;
    uint8_t pressed_state;
    void (*external_isr)();
    uint8_t interrupt_mode;
};


/*
 * sets up the button
 * @param *bp: pointer to button_parameters structure
 */
void button_setup (button_parameters *bp);


int IRAM_ATTR button_interrupt_service_routine (button_parameters *bp);


int button_read_attach_interrupt (button_parameters *bp);