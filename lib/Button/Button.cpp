#include "Button.h"


void button_setup (button_parameters *bp) {
    pinMode(bp->pin, bp->pin_mode);
    attachInterrupt(bp->pin, bp->external_isr, bp->interrupt_mode);
}


int IRAM_ATTR button_interrupt_service_routine (button_parameters *bp) {
    volatile int t_begin;
    
    detachInterrupt(bp->pin);

    // debouncing
    t_begin = millis();
    while (millis() - t_begin < DEBOUNCING_TIME);

    if (digitalRead(bp->pin) != bp->pressed_state) {
        attachInterrupt(bp->pin, bp->external_isr, bp->interrupt_mode);
        return 0;
    }

    return 1;
}


int button_read_attach_interrupt (button_parameters *bp) {
    if (digitalRead(bp->pin) == bp->pressed_state)
        return 1;

    attachInterrupt(bp->pin, bp->external_isr, bp->interrupt_mode);

    return 0;
}