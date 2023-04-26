#include "core1.h"


int read_int_serial();

void gears_calibration() {
    uint8_t end, on, gear;
    char c;
    
    if (!Serial) {
        Serial.begin(9600);
        while (!Serial);
        Serial.println("Serial initialized");
    }

    Serial.println("\nGears calibration");

    stepper_motor.set_direction((cw_direction_sign == 1) ? CCW : CW);
    stepper_motor.set_speed(60);
    while (!limit_reached) {
        stepper_motor.step();
    }
    stepper_motor.set_position(0);

    stepper_motor.change_direction();
    stepper_motor.set_speed(60);

    end = on = gear = 0;
    while (!end) {

        if (on) {
            stepper_motor.step();
        }

        if (Serial.available()) {
            c = Serial.read();

            switch (c) {
                case 'o': on = 1 - on; break;
                case 'c': stepper_motor.change_direction(); break;
                case 's':
                    on = 0;
                    gear = read_int_serial();
                    if (gear < 1 || gear > NUM_GEARS)
                        break;
                    gears[gear-1] = stepper_motor.get_position();
                    Serial.print("Saved gear: "); Serial.print(gear); Serial.print("\tPosition: "); Serial.println(gears[gear-1]);
                    break;
                case 'g':
                    on = 0;
                    gear = read_int_serial();
                    shift(gear);
                    Serial.print("Shifter to gear: "); Serial.println(gear);
                    break;
                case 'e': end = 1; break;
                default: break;
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
                Serial.println("Changes saved");
                end = 1; break;
            default:
                Serial.println("Unrecognized command (use: y,Y/n,N)");
                break;
        }
    }

}


int read_int_serial() {
    String input_str;

    while (!Serial.available()) delay(1);
    
    input_str = Serial.readString();
    input_str.trim();

    for (int i=0; i<input_str.length(); i++) {
        if (!isdigit(input_str[i]))
            return 0;
    }

    return input_str.toInt();
}