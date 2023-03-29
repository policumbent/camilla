#include "Button.h"

Button :: Button (uint8_t pin, uint8_t pin_mode, uint8_t interrupt_mode, uint8_t pressed_state, void(* isr)()) {
    this->pin = pin;
    this->pin_mode = pin_mode;
    this->interrupt_mode = interrupt_mode;
    this->pressed_state = pressed_state;
    this->external_isr = isr;

    setup();
}


void Button :: setup () {
    pinMode(pin, pin_mode);
    attachInterrupt(pin, external_isr, interrupt_mode);

    button_pressed = 0;
}


void IRAM_ATTR Button :: interrupt_service_routine() {
    int t_begin;
    
    detachInterrupt(pin);

    t_begin = millis();
    while (millis() - t_begin < debouncing_time);

    if (digitalRead(pin) != pressed_state) {
        attachInterrupt(pin, external_isr, interrupt_mode);
        return;
    }

    button_pressed++; 
}


void Button :: manage_button() {
    if (!button_pressed)
        return;

    if (digitalRead(pin) == pressed_state)
        return;

    attachInterrupt(pin, external_isr, interrupt_mode);
    button_pressed--;
}