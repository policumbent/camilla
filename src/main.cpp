#include <Arduino.h>

#include "HR4988.h"
#include "Button.h"


#define ENABLE_PIN 18
#define SLEEP_PIN 21
#define RESET_PIN 19

#define STEP_PIN 12
#define DIRECTION_PIN 14

#define MS1_PIN 25
#define MS2_PIN 33
#define MS3_PIN 32

#define LIMIT_SWITCH_PIN 23


const int steps_per_turn = 200;
const float deg_per_full_step = 1.8;
float rpm;

void IRAM_ATTR limit_switch_isr();


HR4988 stepper_motor = HR4988 (
    ENABLE_PIN, SLEEP_PIN, RESET_PIN,
    STEP_PIN, DIRECTION_PIN,
    MS1_PIN, MS2_PIN, MS3_PIN,
    steps_per_turn, deg_per_full_step
);
Button limit_switch = Button (
    LIMIT_SWITCH_PIN, INPUT_PULLUP, FALLING, LOW, limit_switch_isr
);


void setup() {
    Serial.begin(9600);
    while (!Serial);
    Serial.println("Serial initialized correctly");

    rpm = 180;
    stepper_motor.set_speed(rpm);
    stepper_motor.off();
}


void loop() {
    int i;
    
    if (stepper_motor.is_on()) {
        stepper_motor.step();
    }

    if (Serial.available()) {
        char c;
        c = Serial.read();
        //delay(1000);
        switch (c) {
            case 'o': (stepper_motor.is_on()) ? stepper_motor.off() : stepper_motor.on(); break;
            case 's': (stepper_motor.is_sleep()) ? stepper_motor.awake() : stepper_motor.sleep(); break;
            case 'c': stepper_motor.change_direction(); break;
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
        stepper_motor.print_status();
    }

    if (limit_switch.read_attach_interrupt()) {
        Serial.println("Limit reached");

        stepper_motor.change_direction();
        stepper_motor.set_speed(300);
        stepper_motor.set_microstepping(FULL_STEP_MODE);

        while (limit_switch.read_attach_interrupt())
            stepper_motor.step();
    }
}


void IRAM_ATTR limit_switch_isr() {
    limit_switch.interrupt_service_routine();
}