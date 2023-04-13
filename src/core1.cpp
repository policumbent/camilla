#include "main.h"


void function_core_1 (void *parameters) {

    Serial.print("Task 1 initialized running on core: ");
    Serial.println(xPortGetCoreID());

    steps_per_activation = steps_per_turn;
    rpm = 180;
    stepper_motor.set_speed(rpm);
    stepper_motor.off();

    button_setup(&limit_switch_parameters);


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
}


void IRAM_ATTR limit_switch_isr() {
    if (limit_reached = button_interrupt_service_routine(&limit_switch_parameters)) {
        stepper_motor.off();
        limit_reached = xPortGetCoreID() + 1;
    }
}