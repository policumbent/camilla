#include "main.h"

#include "HR4988.h"
#include "Button.h"


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
    steps_per_turn, deg_per_full_step
);

int gears[NUM_GEARS] = {200, 400, 600, 800, 1000, 1200, 1400, 1600, 1800, 2000, 2200, 2400};    // random


uint8_t limit_reached = 0;
void IRAM_ATTR limit_switch_isr();

button_parameters limit_switch_parameters = {
    LIMIT_SWITCH_PIN, INPUT_PULLUP, LOW, limit_switch_isr, FALLING
};


void function_core_1 (void *parameters) {

    Serial.print("Task 1 initialized running on core: "); // debugging purposes
    Serial.println(xPortGetCoreID());                     // debugging purposes

    stepper_motor.on();

    button_setup(&limit_switch_parameters);

    // TODO: load gear from flash into gears array
    stepper_motor.set_direction(CCW);   // TODO: to be checked
    stepper_motor.set_microstepping(SIXTEENTH_STEP_MODE);
    stepper_motor.set_speed(20);
    while (!limit_reached) {
        stepper_motor.step();
    }
    stepper_motor.set_position(0);
    // TODO: move to 1st gear
    limit_reached = button_read_attach_interrupt(&limit_switch_parameters);


    while (1) {



    }
}


void IRAM_ATTR limit_switch_isr() {
    if (limit_reached = button_interrupt_service_routine(&limit_switch_parameters)) {
        stepper_motor.off();
        limit_reached = xPortGetCoreID() + 1;   // debugging purposes
    }
}



/*  TEST CODE

    while (1) {

        if (stepper_motor.is_on()) {
            stepper_motor.step();
            current_steps++;

            if (current_steps >= steps_per_activation) {
                stepper_motor.off();
            }
        }

        if (Serial.available()) {
            char c;
            c = Serial.read();

            switch (c) {
                case 'o': (stepper_motor.is_on()) ? stepper_motor.off() : stepper_motor.on();
                    current_steps = 0; break;
                case 's': (stepper_motor.is_sleep()) ? stepper_motor.awake() : stepper_motor.sleep(); break;
                case 'c': stepper_motor.change_direction(); break;
                case '^': steps_per_activation += 1; break;
                case 'i': steps_per_activation += 10; break;
                case 'I': steps_per_activation += 100; break;
                case 'v': steps_per_activation -= 1; break;
                case 'd': steps_per_activation -= 10; break;
                case 'D': steps_per_activation -= 100; break;
                case '*': rpm += 100; stepper_motor.set_speed(rpm); break;
                case '/': rpm -= 100; stepper_motor.set_speed(rpm); break;
                case '+': rpm += 10; stepper_motor.set_speed(rpm); break;
                case '-': rpm -= 10; stepper_motor.set_speed(rpm); break;
                case ',': rpm += 1; stepper_motor.set_speed(rpm); break;
                case '.': rpm -= 1; stepper_motor.set_speed(rpm); break;
                case '1': stepper_motor.set_microstepping(FULL_STEP_MODE); break;
                case '2': stepper_motor.set_microstepping(HALF_STEP_MODE); break;
                case '4': stepper_motor.set_microstepping(QUARTER_STEP_MODE); break;
                case '8': stepper_motor.set_microstepping(EIGHT_STEP_MODE); break;
                case '6': stepper_motor.set_microstepping(SIXTEENTH_STEP_MODE); break;
                default: break;
            }
            if (xSemaphoreTake(semaphore, DELAY_WAIT_SEMAPHORE)) {
                stepper_motor.print_status();
                sprintf(message, "Steps per activation: %d\t", steps_per_activation);
                print = 1;
                xSemaphoreGive(semaphore);
            }
        }

        if (limit_reached) {
            Serial.println("Limit reached");
            Serial.print("The isr has runned on core: ");
            Serial.println(limit_reached - 1);

            stepper_motor.change_direction();
            stepper_motor.set_microstepping(FULL_STEP_MODE);
            stepper_motor.set_speed(300);
            stepper_motor.on();

            while (limit_reached = button_read_attach_interrupt(&limit_switch_parameters)) {
                stepper_motor.step();
            }

            stepper_motor.change_direction();
        }
    }

*/