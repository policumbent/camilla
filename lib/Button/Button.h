#pragma once
#include <Arduino.h>


class Button {

    private:
        uint8_t pin;
        uint8_t pin_mode;
        uint8_t interrupt_mode;
        uint8_t pressed_state;
        const int debouncing_time = 50;
        void (* external_isr)();
        uint8_t button_pressed;

    public:
        Button (uint8_t pin, uint8_t pin_mode, uint8_t interrupt_mode, uint8_t pressed_state, void(* isr)());
        void setup();

        void IRAM_ATTR interrupt_service_routine();
        void manage_button();
};