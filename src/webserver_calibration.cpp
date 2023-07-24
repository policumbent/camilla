#include "core1.h"
#include "core0.h"


#if !SERIAL_CALIBRATION


#include "WebServer.h"


void calibration() {
    const int SPEED = 100;
    uint8_t end;

    stepper_motor.set_direction(NEGATIVE_DIR);
    stepper_motor.set_speed(SPEED);
    while (!limit_reached) {
        stepper_motor.step();
    }
    stepper_motor.set_position(0);

    stepper_motor.change_direction();
    stepper_motor.set_speed(SPEED);

    shift_up_pressed = shift_down_pressed = calibration_button_pressed = 0;

    end = 0;

    while (!end) {

        if (shift_up_pressed) {
            stepper_motor.set_direction(POSITIVE_DIR);
            stepper_motor.move_while_button_pressed(SPEED, &shift_up_pressed, &shift_up_button_parameters);
        }

        if (shift_down_pressed) {
            stepper_motor.set_direction(NEGATIVE_DIR);
            stepper_motor.move_while_button_pressed(SPEED, &shift_down_pressed, &shift_down_button_parameters);
        }

        if (limit_reached) {
            stepper_motor.change_direction();
            stepper_motor.move_while_button_pressed(SPEED, &limit_reached, &limit_switch_parameters);
        }

        if (calibration_button_pressed) {
            end = 1;
        }

    }
}


void webserver_calibration() {
    int *gears_ptr = gears;
    int *gears_lin_ptr = gears_lin;
    WebServer webserver = WebServer(&stepper_motor, &linear_potentiometer, gears_ptr, gears_lin_ptr, NUM_GEARS);

    while (!calibration_button_pressed) delay(10);

    flash.write_array(gears_memory_key, (void *) gears, NUM_GEARS, sizeof(int));
    flash.write_array(gears_lin_memory_key, (void *) gears_lin, NUM_GEARS, sizeof(int));
}

#endif