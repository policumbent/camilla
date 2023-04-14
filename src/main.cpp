#include "main.h"


SemaphoreHandle_t semaphore;

TaskHandle_t task_core_1;
TaskHandle_t task_core_0;


void setup() {
    Serial.begin(9600);
    while (!Serial);
    Serial.println("Serial initialized");

    semaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(semaphore);

    xTaskCreatePinnedToCore(function_core_1, "Core_1", 10000, NULL, 0, &task_core_1, 1);
    delay(500);
    xTaskCreatePinnedToCore(function_core_0, "Core_0", 10000, NULL, 0, &task_core_0, 0);
    delay(500);
}


void loop() {
    delay(10000);
}