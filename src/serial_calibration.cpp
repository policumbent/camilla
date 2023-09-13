#include "core1.h"


#if SERIAL_CALIBRATION


void gears_calibration();
int read_int_serial();


void calibration() {
    if (!Serial) {
        Serial.begin(9600);
        while (!Serial);
        Serial.println("Serial initialized");
    }

    char c;
    uint8_t end = 0;

    Serial.println("Select: Gears, Encoder, Potentiometer or Driver calibration (g,G / e,E / p,P / d,D)");

    while (!end) {
        while (!Serial.available()) delay(1);
        c = Serial.read();
        switch (c) {
            case 'g': case 'G':
                gears_calibration();
                end = 1; break;
            case 'e': case 'E':
                rotative_encoder.calibration(stepper_motor);
                end = 1; break;
            case 'p': case 'P':
                linear_potentiometer.calibration(stepper_motor);
                end = 1; break;
            case 'd': case 'D':
                stepper_motor.driver_calibration();
                end = 1; break;
            default:
                Serial.println("Unrecognized command (use: g,G / e,E / p,P / d,D)");
                break;
        }
    }    
}


void gears_calibration() {
    uint8_t end, gear;
    char c;

    Serial.println("\nGears calibration");

    while (!shift_down_pressed) delay(10);
    while ((shift_down_pressed = button_read_attach_interrupt(&shift_down_button_parameters)));

    stepper_motor.go_to_limit_switch(zero_reference_limit_switch_type);
    stepper_motor.set_position(0);

    shift_up_pressed = shift_down_pressed = calibration_button_pressed = 0;

    end = gear = 0;

    while (!end) {

        if (shift_up_pressed) {
            stepper_motor.move_while_button_pressed_check_limit_switches(HR4988_POSITIVE_DIR, &shift_up_pressed, &shift_up_button_parameters);
        }

        if (shift_down_pressed) {
            stepper_motor.move_while_button_pressed_check_limit_switches(HR4988_NEGATIVE_DIR, &shift_down_pressed, &shift_down_button_parameters);
        }

        if (switch_begin_pressed) {
            stepper_motor.move_while_button_pressed(HR4988_POSITIVE_DIR, &switch_begin_pressed, &limit_switch_begin_parameters, DISTANCE_FROM_LIMIT_SWITCHES);
        }

        if (switch_end_pressed) {
            stepper_motor.move_while_button_pressed(HR4988_NEGATIVE_DIR, &switch_end_pressed, &limit_switch_end_parameters, DISTANCE_FROM_LIMIT_SWITCHES);
        }

        if (calibration_button_pressed) {
            end = 1;
        }

        if (Serial.available()) {
            c = Serial.read();

            switch (c) {

                case '+':
                    stepper_motor.set_direction(HR4988_POSITIVE_DIR);
                    for (int i=0; i<16; i++) stepper_motor.step();
                    break;

                case '*':
                    stepper_motor.set_direction(HR4988_POSITIVE_DIR);
                    for (int i=0; i<160; i++) stepper_motor.step();
                    break;

                case '-':
                    stepper_motor.set_direction(HR4988_NEGATIVE_DIR);
                    for (int i=0; i<16; i++) stepper_motor.step();
                    break;

                case '/':
                    stepper_motor.set_direction(HR4988_NEGATIVE_DIR);
                    for (int i=0; i<160; i++) stepper_motor.step();
                    break;

                case 's': case 'S':
                    Serial.println("Enter the number of the gear to be saved: ");
                    gear = read_int_serial();
                    if (gear < 1 || gear > NUM_GEARS)
                        break;

                    gears[gear-1] = stepper_motor.get_position();
                    gears_lin[gear-1] = linear_potentiometer.read_position();

                    Serial.println("GEAR SAVED");
                    Serial.print(gear); Serial.print("\tPosition: "); Serial.println(gears[gear-1]);
                    Serial.print("Linear position: "); Serial.println(gears_lin[gear-1]);
                    Serial.println();
                    break;

                case 'g': case 'G':
                    Serial.println("Enter the number of the gear to shift to: ");
                    gear = read_int_serial();
                    shift(gear);
                    Serial.print("Shifted to gear: "); Serial.println(gear);
                    break;

                case 'e': case 'E':
                    end = 1; break;

                default:
                    break;
            }
        }
    }

    Serial.println("Confirm changes? (y/n)");
    end = 0;
    while (!end) {
        while (!Serial.available()) delay(1);
        c = Serial.read();
        switch (c) {
            case 'n': case 'N':
                Serial.println("Changes NOT saved");
                end = 1; break;

            case 'y': case 'Y':
                flash.write_array(gears_memory_key, (void *) gears, NUM_GEARS, sizeof(int));
                flash.write_array(gears_lin_memory_key, (void *) gears_lin, NUM_GEARS, sizeof(int));
                Serial.println("Changes saved");
                end = 1; break;

            default:
                Serial.println("Unrecognized command (use: y,Y/n,N)"); break;
        }
    }

    flash.read_array(gears_memory_key, (void *) gears, NUM_GEARS, sizeof(int));
    flash.read_array(gears_lin_memory_key, (void *) gears_lin, NUM_GEARS, sizeof(int));

    #if DEBUG_CALIBRATION
        Serial.print("CALIBRATION ENDED\nGears saved in memory:\n");
        char str_cal[50];
        for (int i=0; i<NUM_GEARS; i++) {
            sprintf(str_cal, "Gear: %d \tPosition: %d \tLinear position: %d", i+1, gears[i], gears_lin[i]);
            Serial.println(str_cal);
        }
    #endif
}


int read_int_serial() {
    String input_str;

    while (!Serial.available()) delay(1);
    
    input_str = Serial.readString();
    input_str.trim();

    for (int i=0; i<input_str.length(); i++) {
        if (!isdigit(input_str[i])) return 0;
    }

    return input_str.toInt();
}

void webserver_calibration() {
    // just to not have a #if in core0.cpp
}
#endif