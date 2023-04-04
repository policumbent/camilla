#include <Arduino.h>

#include "HR4988.h"
#include "Button.h"
#include "Potentiometer.h"

#define DELAY_WAIT_SEMAPHORE 10


#define ENABLE_PIN 18
#define SLEEP_PIN 21
#define RESET_PIN 19

#define STEP_PIN 12
#define DIRECTION_PIN 14

#define MS1_PIN 25
#define MS2_PIN 33
#define MS3_PIN 32

#define LIMIT_SWITCH_PIN 23

#define POTENTIOMETER_PIN 34


const int steps_per_turn = 200;
const float deg_per_full_step = 1.8;

float rpm;
int steps_per_activation;
int current_steps = 0;

uint8_t limit_reached = 0;

void IRAM_ATTR limit_switch_isr();

char message[100];
uint8_t print = 0;


HR4988 stepper_motor = HR4988 (
    ENABLE_PIN, SLEEP_PIN, RESET_PIN,
    STEP_PIN, DIRECTION_PIN,
    MS1_PIN, MS2_PIN, MS3_PIN,
    steps_per_turn, deg_per_full_step
);
button_parameters limit_switch_parameters = {
    LIMIT_SWITCH_PIN, INPUT_PULLUP, LOW, limit_switch_isr, FALLING
};
Potentiometer potentiometer = Potentiometer (
    POTENTIOMETER_PIN
);


SemaphoreHandle_t semaphore;

TaskHandle_t task_core_1;
TaskHandle_t task_core_0;

void function_core_1 (void *parameters);
void function_core_0 (void *parameters);


void setup() {
    Serial.begin(9600);
    while (!Serial);
    Serial.println("Serial initialized");

    steps_per_activation = steps_per_turn;
    rpm = 180;
    stepper_motor.set_speed(rpm);
    stepper_motor.off();

    button_setup(&limit_switch_parameters);

    semaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(semaphore);

    xTaskCreatePinnedToCore(function_core_1, "Core_1", 10000, NULL, 0, &task_core_1, 1);
    delay(500);
    xTaskCreatePinnedToCore(function_core_0, "Core_0", 10000, NULL, 0, &task_core_0, 0);
    delay(500);
}


void function_core_1 (void *parameters) {

    Serial.print("Task 1 initialized running on core: ");
    Serial.println(xPortGetCoreID());


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
                case 'p': potentiometer.calibration(stepper_motor); break;
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


void function_core_0 (void *parameters) {

    Serial.print("Task 0 initialized running on core: ");
    Serial.println(xPortGetCoreID());


    while (1) {
    
        if (print && xSemaphoreTake(semaphore, portMAX_DELAY)) {
            Serial.print(message);
            stepper_motor.print_status();
            print = 0;
            xSemaphoreGive(semaphore);
        }
        delay(100);

    }
}


void loop() {
    delay(10000);
}


void IRAM_ATTR limit_switch_isr() {
    if (limit_reached = button_interrupt_service_routine(&limit_switch_parameters)) {
        stepper_motor.off();
        limit_reached = xPortGetCoreID() + 1;
    }
}