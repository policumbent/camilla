#include "main.h"


TaskHandle_t task_core_1;
TaskHandle_t task_core_0;

SemaphoreHandle_t g_sem_move;
SemaphoreHandle_t g_sem_pos;

uint8_t g_current_gear;

HR4988 stepper_motor = HR4988 (
    STEP_PIN, DIRECTION_PIN,
    MS1_PIN, MS2_PIN, MS3_PIN,
    ENABLE_PIN,
    full_steps_per_turn, deg_per_full_step,
    cw_direction_sign
);


AS5600 rotative_encoder = AS5600 (
    MAGNETIC_ENCODER_PIN
);


Potentiometer linear_potentiometer = Potentiometer (
    POTENTIOMETER_PIN
);


int gears[NUM_GEARS];


Memory flash = Memory();


void setup() {
    #if DEBUG
        Serial.begin(9600);
        while (!Serial);
        Serial.println("Serial initialized");
    #endif

    g_sem_move = xSemaphoreCreateBinary();
    g_sem_pos = xSemaphoreCreateBinary();
    xSemaphoreGive(g_sem_pos);

    xTaskCreatePinnedToCore(function_core_1, "Core_1", 10000, NULL, configMAX_PRIORITIES-1, &task_core_1, 1);
    delay(500);
    xTaskCreatePinnedToCore(function_core_0, "Core_0", 10000, NULL, configMAX_PRIORITIES-1, &task_core_0, 0);
    delay(500);
}


void loop() {
    delay(10000);
}