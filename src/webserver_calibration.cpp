#include "core1.h"
#include "core0.h"


#if !SERIAL_CALIBRATION


#include "WebServer.h"


void calibration() {
    uint8_t end;

    #if DEBUG_CALIBRATION
        Serial.println("WEBSERVER CALIBRATION");
    #endif

    while (!shift_down_pressed) delay(10);
    while ((shift_down_pressed = button_read_attach_interrupt(&shift_down_button_parameters)));

    stepper_motor.go_to_limit_switch(zero_reference_limit_switch_type);
    stepper_motor.set_position(0);

    shift_up_pressed = shift_down_pressed = 0;

    end = 0;

    while (!end) {

        xSemaphoreTake(g_semaphore, portMAX_DELAY);

        if (shift_up_pressed) {
            stepper_motor.move_while_button_pressed_check_limit_switches(HR4988_POSITIVE_DIR, &shift_up_pressed, &shift_up_button_parameters);
        }

        if (shift_down_pressed) {
            stepper_motor.move_while_button_pressed_check_limit_switches(HR4988_NEGATIVE_DIR, &shift_down_pressed, &shift_down_button_parameters);
        }

        if (switch_begin_pressed) {
            stepper_motor.move_while_button_pressed(LIMIT_SWITCH_PRESSED_SPEED, HR4988_POSITIVE_DIR, &switch_begin_pressed, &limit_switch_begin_parameters, DISTANCE_FROM_LIMIT_SWITCHES);
        }

        if (switch_end_pressed) {
            stepper_motor.move_while_button_pressed(LIMIT_SWITCH_PRESSED_SPEED, HR4988_NEGATIVE_DIR, &switch_end_pressed, &limit_switch_end_parameters, DISTANCE_FROM_LIMIT_SWITCHES);
        }

        if (calibration_button_pressed) {
            end = 1;
        }

        xSemaphoreGive(g_semaphore);

        delay(10);

    }
}


void webserver_calibration() {
    int *gears_ptr = gears;
    int *gears_lin_ptr = gears_lin;
    WebServer webserver = WebServer(&stepper_motor, &linear_potentiometer, gears_ptr, gears_lin_ptr, NUM_GEARS, &g_semaphore);

    blink_built_in_led(4);

    while (!calibration_button_pressed) delay(100);

    flash.write_array(gears_memory_key, (void *) gears, NUM_GEARS, sizeof(int));
    flash.write_array(gears_lin_memory_key, (void *) gears_lin, NUM_GEARS, sizeof(int));

    #if DEBUG_CALIBRATION
        Serial.print("CALIBRATION ENDED\nGears saved in memory:\n");
        char str_cal[50];
        for (int i=0; i<NUM_GEARS; i++) {
            sprintf(str_cal, "Gear: %d \tPosition: %d \tLinear position: %d", i+1, gears[i], gears_lin[i]);
            Serial.println(str_cal);
        }
    #endif
}

#endif