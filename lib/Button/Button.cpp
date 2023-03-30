#include "Button.h"


void button_setup (button_parameters *bp) {
    pinMode(bp->pin, bp->pin_mode);
    attachInterrupt(bp->pin, bp->external_isr, bp->interrupt_mode);

    *(bp->is_pressed) = 0;
}


void IRAM_ATTR button_interrupt_service_routine (button_parameters *bp) {
    volatile int t_begin;
    
    detachInterrupt(bp->pin);

    t_begin = millis();
    while (millis() - t_begin < debouncing_time);

    if (digitalRead(bp->pin) != bp->pressed_state) {
        attachInterrupt(bp->pin, bp->external_isr, bp->interrupt_mode);
        return;
    }

    *(bp->is_pressed)++; 
}


int read_attach_interrupt (button_parameters *bp) {
    if (!*(bp->is_pressed))
        return 0;

    if (digitalRead(bp->pin) == *(bp->is_pressed))
        return 1;

    attachInterrupt(bp->pin, bp->external_isr, bp->interrupt_mode);
    *(bp->is_pressed)--;

    return *(bp->is_pressed);
}