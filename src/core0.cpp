#include "core0.h"


uint16_t angle;
uint16_t linear_position;
uint8_t cnt;


void function_core_0 (void *parameters) {

#if DEBUG_CORES
    Serial.print("Task 0 initialized running on core: ");
    Serial.println(xPortGetCoreID());
#endif

    cnt = 0;

    delay(1000);

    while (1) {

        xSemaphoreTake(g_sem_move, portMAX_DELAY);

        angle = rotative_encoder.read_angle();

        xSemaphoreTake(g_sem_pos, portMAX_DELAY);
        int pos = stepper_motor.get_position();
        xSemaphoreGive(g_sem_pos);
        
        if (cnt % 10 == 0) {
            linear_position = linear_potentiometer.read_position();
            xSemaphoreTake(g_sem_pos, portMAX_DELAY);
            int pos = stepper_motor.get_position();
            xSemaphoreGive(g_sem_pos);
        }
        cnt = (cnt + 1) % 10;

        xSemaphoreGive(g_sem_move);

        #if DEBUG_FEEDBACK >= 2
            Serial.print("Angle: "); Serial.print(angle);
            Serial.print("\tLinear: "); Serial.print(linear_position);
            Serial.print("\tCnt: "); Serial.println(cnt);
        #endif
    }
}