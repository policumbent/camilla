#include "main.h"


TaskHandle_t task_core_1;
TaskHandle_t task_core_0;

SemaphoreHandle_t g_semaphore;

uint8_t g_current_gear;


void setup() {
    Serial.begin(9600);
    while (!Serial);
    Serial.println("Serial initialized");

    g_semaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(g_semaphore);

    xTaskCreatePinnedToCore(function_core_1, "Core_1", 10000, NULL, 0, &task_core_1, 1);
    delay(500);
    xTaskCreatePinnedToCore(function_core_0, "Core_0", 10000, NULL, 0, &task_core_0, 0);
    delay(500);
}


void loop() {
    delay(10000);
}