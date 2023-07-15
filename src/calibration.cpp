#include "core1.h"


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

    Serial.println("Select: Gears, Encoder or Potentiometer calibration (g,G/e,E/p,P)");

    while(!end) {
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
            default:
                Serial.println("Unrecognized command (use: g,G/e,E/p,P)");
                break;
        }
    }    
}


void gears_calibration() {
    uint8_t end, on, gear;
    char c;

    Serial.println("\nGears calibration");

    stepper_motor.set_direction(NEGATIVE_DIR);
    stepper_motor.set_speed(60);
    while (!limit_reached) {
        stepper_motor.step();
    }
    stepper_motor.set_position(0);

    stepper_motor.change_direction();
    stepper_motor.set_speed(60);

    shift_up_pressed = shift_down_pressed = calibration_button_pressed = 0;

    end = on = gear = 0;
    while (!end) {

        if (on) {
            stepper_motor.step();
        }

        if (shift_up_pressed) {
            on = 0;
            shift(g_current_gear + 1);
            Serial.println(g_current_gear);
            while ((shift_up_pressed = button_read_attach_interrupt(&shift_up_button_parameters)));
        }

        if (shift_down_pressed) {
            on = 0;
            shift(g_current_gear - 1);
            Serial.println(g_current_gear);
            while ((shift_down_pressed = button_read_attach_interrupt(&shift_down_button_parameters)));
        }

        if (limit_reached) {
            on = 0;
            stepper_motor.change_direction();
            stepper_motor.set_speed(MIN_MOVE_RPM);
            while ((limit_reached = button_read_attach_interrupt(&limit_switch_parameters))) {
                stepper_motor.step();
            }
        }

        if (calibration_button_pressed) {
            end = 1;
            while ((calibration_button_pressed = button_read_attach_interrupt(&calibration_button_parameters)));
        }

        if (Serial.available()) {
            c = Serial.read();

            switch (c) {
                case 'o':
                    on = 1 - on; break;

                case 'c':
                    stepper_motor.change_direction(); break;

                case '+':
                    for (int i=0; i<10; i++) stepper_motor.step();
                    break;

                case '*':
                    for (int i=0; i<100; i++) stepper_motor.step();
                    break;

                case 's':
                    on = 0;
                    gear = read_int_serial();
                    if (gear < 1 || gear > NUM_GEARS)
                        break;

                    gears[gear-1] = stepper_motor.get_position();
                    gears_lin[gear-1] = linear_potentiometer.read_position();

                    Serial.println("GEAR SAVED");
                    Serial.print(gear); Serial.print("\tPosition: "); Serial.println(gears[gear-1]);
                    Serial.print("Linear position: "); Serial.println(gears_lin[gear-1]);
                    break;

                case 'g':
                    on = 0;
                    gear = read_int_serial();
                    shift(gear);
                    Serial.print("Shifting to gear: "); Serial.println(gear);
                    break;

                case 'e':
                    end = 1; break;

                default:
                    break;
            }
        }
    }

    Serial.println("Confirm changes? (Y/n)");
    end = 0;
    while(!end) {
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