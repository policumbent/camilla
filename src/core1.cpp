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
int gears[NUM_GEARS] = {200, 400, 600, 800, 1000, 1200, 1400, 1600, 1800, 2000, 2200, 2400};


uint8_t limit_reached = 0;
void IRAM_ATTR limit_switch_isr();

button_parameters limit_switch_parameters = {
    LIMIT_SWITCH_PIN, INPUT_PULLUP, LOW, limit_switch_isr, FALLING
};


Memory flash = Memory();


void function_core_1 (void *parameters) {

#if DEBUG_CORES
    Serial.print("Task 1 initialized running on core: ");
    Serial.println(xPortGetCoreID());
#endif

    button_setup(&limit_switch_parameters);

    // For debugging try to load and read gears
    flash.write_gears(gears, NUM_GEARS);
    
    flash.read_gears(gears, NUM_GEARS);
    stepper_motor.set_direction(CCW);   // TODO: to be checked
    stepper_motor.set_microstepping(SIXTEENTH_STEP_MODE);
    stepper_motor.set_speed(20);
    while (!limit_reached) {
        stepper_motor.step();
    }
    stepper_motor.set_position(0);
    // TODO: move to 1st gear
#if DEBUG_CORES
    Serial.print("The limit switch isr has runned on core: ");
    Serial.println(limit_reached - 1);
#endif
    limit_reached = button_read_attach_interrupt(&limit_switch_parameters);

    stepper_motor.debug_serial_control();

    while (1) {

        // TODO: wait for button pression (upshift or downshift)
        // TODO: shift gear if needed
        // TODO: signal to core0 the need to pass the gear to CAN-BUS
        delay(100);

    }
}


void IRAM_ATTR limit_switch_isr() {
    limit_reached = button_interrupt_service_routine(&limit_switch_parameters);
#if DEBUG_CORES
    if (limit_reached)
        limit_reached = xPortGetCoreID() + 1;
#endif
}


void shift(uint8_t delta) {
    uint8_t next_gear = g_current_gear + delta;

    if (next_gear < 0 || next_gear > NUM_GEARS)
        return;

    int current_pos, start_pos, target_pos;
    current_pos = start_pos = stepper_motor.get_position();
    target_pos = gears[next_gear];

    while (current_pos != target_pos) {
        // TODO: include encoder feedback and position correction
        stepper_motor.move(start_pos, target_pos);
    }
}