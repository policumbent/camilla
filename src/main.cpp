#include "main.h"


TaskHandle_t task_core_1;
TaskHandle_t task_core_0;

SemaphoreHandle_t g_semaphore;

uint8_t g_current_gear;
uint8_t g_calibration_flag;


uint8_t switch_begin_pressed = 0;
uint8_t switch_end_pressed = 0;
void IRAM_ATTR limit_switch_begin_isr();
void IRAM_ATTR limit_switch_end_isr();

button_parameters limit_switch_begin_parameters = {
    LIMIT_SWITCH_BEGIN_PIN, INPUT_PULLUP, LOW, limit_switch_begin_isr, FALLING
};
button_parameters limit_switch_end_parameters = {
    LIMIT_SWITCH_END_PIN, INPUT_PULLUP, LOW, limit_switch_end_isr, FALLING
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


uint8_t calibration_button_pressed = 0;
void IRAM_ATTR calibration_button_isr();

button_parameters calibration_button_parameters = {
    CALIBRATION_BUTTON_PIN, INPUT_PULLUP, LOW, calibration_button_isr, FALLING
};


void setup() {
    #if DEBUG
        Serial.begin(9600);
        while (!Serial) delay(10);
        delay(2000);
        Serial.println("Serial initialized");
    #endif

    pinMode(BUILT_IN_LED_PIN, OUTPUT);
    digitalWrite(BUILT_IN_LED_PIN, LOW);

    g_semaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(g_semaphore);

    g_current_gear = 0;
    g_calibration_flag = 0;

    xTaskCreatePinnedToCore(function_core_1, "Core_1", 20000, NULL, configMAX_PRIORITIES-1, &task_core_1, 1);
    delay(500);
    xTaskCreatePinnedToCore(function_core_0, "Core_0", 10000, NULL, configMAX_PRIORITIES-1, &task_core_0, 0);
    delay(500);
}


void loop() {
    delay(10000);
}


void blink_built_in_led(uint8_t n_times) {
    for (uint8_t i=0; i<n_times; i++) {
        digitalWrite(BUILT_IN_LED_PIN, HIGH);
        delay(125);
        digitalWrite(BUILT_IN_LED_PIN, LOW);
        delay(125);
    }
}