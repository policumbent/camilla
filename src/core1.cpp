#include "main.h"

#include "HR4988.h"
#include "Button.h"
#include "Memory.h"


#define NUM_GEARS        12     // for Phoenix


#define ENABLE_PIN       18
#define SLEEP_PIN        21
#define RESET_PIN        19

#define STEP_PIN         12
#define DIRECTION_PIN    14

// microstepping pins
#define MS1_PIN          25
#define MS2_PIN          33
#define MS3_PIN          32


#define LIMIT_SWITCH_PIN 23

#define SHIFT_UP_BUTTON_PIN   34    // NOT CORRECT
#define SHIFT_DOWN_BUTTON_PIN 35    // NOT CORRECT



const int steps_per_turn = 200;
const float deg_per_full_step = 1.8;

HR4988 stepper_motor = HR4988 (
    ENABLE_PIN, SLEEP_PIN, RESET_PIN,
    STEP_PIN, DIRECTION_PIN,
    MS1_PIN, MS2_PIN, MS3_PIN,
    steps_per_turn, deg_per_full_step, +1
);

/*
 * for now, these are random values, consider them to be in sixteenth of step
 */
int gears[NUM_GEARS];


uint8_t limit_reached = 0;
void IRAM_ATTR limit_switch_isr();

button_parameters limit_switch_parameters = {
    LIMIT_SWITCH_PIN, INPUT_PULLUP, LOW, limit_switch_isr, FALLING
};


uint8_t shift_up_pressed = 0;
uint8_t shift_down_pressed = 0;
void IRAM_ATTR shift_up_button_isr();
void IRAM_ATTR shift_down_button_isr();

button_parameters shift_up_button_parameters = {
    SHIFT_UP_BUTTON_PIN, INPUT_PULLUP, LOW, shift_up_button_isr, FALLING
};
button_parameters shift_down_button_parameters = {
    SHIFT_DOWN_BUTTON_PIN, INPUT_PULLUP, LOW, shift_down_button_isr, FALLING
};


const char gears_memory_key[] = "gears";

Memory flash = Memory();


void shift(uint8_t delta);


void function_core_1 (void *parameters) {

    #if DEBUG_CORES
        Serial.print("Task 1 initialized running on core: ");
        Serial.println(xPortGetCoreID());
    #endif

    button_setup(&limit_switch_parameters);

    button_setup(&shift_up_button_parameters);
    button_setup(&shift_down_button_parameters);

    #if DEBUG_MEMORY >= 2
        for (int i=0; i<NUM_GEARS; i++)
            gears[i] = steps_per_turn * 16 * (i+1);
        flash.write_array(gears_memory_key, (void *) gears, NUM_GEARS, sizeof(int));
        for (int i=0; i<NUM_GEARS; i++)
            gears[i] = 0;
    #endif    

    flash.read_array(gears_memory_key, (void *) gears, NUM_GEARS, sizeof(int));

    #if DEBUG_MEMORY
        Serial.println("Gears read from memory");
        for (int i=0; i<NUM_GEARS; i++) {
            char str[50];
            sprintf(str, "Gear: %d \tPosition: %d", i+1, gears[i]);
            Serial.println(str);
        }
    #endif

    stepper_motor.set_direction(CCW);   // TODO: to be checked
    stepper_motor.set_speed(100);
    while (!limit_reached) {
        stepper_motor.step();
    }
    stepper_motor.set_position(0);

    g_current_gear = 1;
    shift(0);
    
    #if DEBUG_CORES
        Serial.print("The limit switch isr has runned on core: ");
        Serial.println(limit_reached - 1);
    #endif

    while (limit_reached) {
        limit_reached = button_read_attach_interrupt(&limit_switch_parameters);
    }

    #if DEBUG_MOTOR
        stepper_motor.debug_serial_control();
    #endif

    while (1) {

        if (shift_up_pressed) {
            #if DEBUG_BUTTONS
                Serial.println("Shifting up");
            #endif
            shift(+1);

            while (shift_up_pressed) {
                shift_up_pressed = button_read_attach_interrupt(&shift_up_button_parameters);
            }
        }

        if (shift_down_pressed) {
            #if DEBUG_BUTTONS
                Serial.println("Shifting down");
            #endif
            shift(-1);

            while (shift_down_pressed) {
                shift_down_pressed = button_read_attach_interrupt(&shift_down_button_parameters);
            }
        }
        
        // TODO: signal to core0 the need to pass the gear to CAN-BUS

    }
}


void IRAM_ATTR limit_switch_isr() {
    limit_reached = button_interrupt_service_routine(&limit_switch_parameters);
    #if DEBUG_CORES
        if (limit_reached)
            limit_reached = xPortGetCoreID() + 1;
    #endif
}


void IRAM_ATTR shift_up_button_isr() {
    shift_up_pressed = button_interrupt_service_routine(&shift_up_button_parameters);
}


void IRAM_ATTR shift_down_button_isr() {
    shift_down_pressed = button_interrupt_service_routine(&shift_down_button_parameters);
}


void shift(uint8_t delta) {
    uint8_t next_gear = g_current_gear + delta;

    if (next_gear < 1 || next_gear > NUM_GEARS)
        return;

    int start_pos, target_pos;
    start_pos = stepper_motor.get_position();
    target_pos = gears[next_gear-1];

    while (stepper_motor.get_position() != target_pos) {
        // TODO: include encoder feedback and position correction
        stepper_motor.move(start_pos, target_pos);
    }

    g_current_gear += delta;
}