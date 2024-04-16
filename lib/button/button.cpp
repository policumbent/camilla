#include "button.h"


void button_setup(button_parameters *bp) {
    pinMode(bp->pin, bp->pin_mode);
    attachInterrupt(bp->pin, bp->external_isr, bp->interrupt_mode);
}


int IRAM_ATTR button_interrupt_service_routine(button_parameters *bp) {
    volatile int t_begin;
    
    detachInterrupt(bp->pin);

    // debouncing
    t_begin = millis();
    while (millis() - t_begin < BUTTON_DEBOUNCING_TIME);

    if (digitalRead(bp->pin) != bp->pressed_state) {
        attachInterrupt(bp->pin, bp->external_isr, bp->interrupt_mode);
        return 0;
    }

    return 1;
}

int button_read_attach_interrupt(button_parameters *bp) {
    if (digitalRead(bp->pin) == bp->pressed_state)
        return 1;

    attachInterrupt(bp->pin, bp->external_isr, bp->interrupt_mode);

    return 0;
}


uint8_t check_long_press(uint8_t *button_pressed, button_parameters *bp, int time) {
    int start_time;
    uint8_t ret = 0;

    if (*button_pressed) {
        digitalWrite(BUILTIN_LED, HIGH);
        start_time = millis();
        while ((*button_pressed = button_read_attach_interrupt(bp))) {
            if (millis() - start_time >= time) {
                ret = 1;
            }
        }
        digitalWrite(BUILTIN_LED, LOW);
    }

    return ret;
}