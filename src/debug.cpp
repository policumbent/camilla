#include "core1.h"


void debug() {
#if DEBUG_MEMORY
    Serial.println("Gears read from memory");
    char str_mem[50];
    for (int i=0; i<NUM_GEARS; i++) {
        sprintf(str_mem, "Gear: %d \tPosition: %d \tLinear position: %d", i+1, gears[i], gears_lin[i]);
        Serial.println(str_mem);
    }
    Serial.print("Direction change offset: "); Serial.println(dir_change_offset[0]);
#endif

#if DEBUG_MOTOR
    stepper_motor.set_speed(60);
    
    char str_mot[100];
    unsigned long int t_mot;
    
    for (int i=0; i<10; i++) {
        t_mot = micros();
        stepper_motor.step();
        t_mot = micros() - t_mot;
        
        sprintf(str_mot, "Measured delay per step: %ld\tExpected delay per step: %ld",
            time, stepper_motor.get_expected_step_time());
        Serial.println(str_mot);
    }

    t_mot = micros();
    for (int i=0; i<10000; i++) {
        stepper_motor.step();
    }
    t_mot = micros() - t_mot;
    sprintf(str_mot, "Measured delay per step: %lf\tExpected delay per step: %ld",
        (double) t_mot / 10000.0, stepper_motor.get_expected_step_time());
    Serial.println(str_mot);
#endif

#if DEBUG_ENCODER
    char str_enc[100];
    unsigned long int t_enc;
    uint16_t angle;

    t_enc = micros();
    for (int i=0; i<1000; i++) {
        angle = rotative_encoder.read_angle();
    }
    t_enc = micros() - t_enc;

    sprintf(str_enc, "Rotative encoder average read time: %f\t", (float) t_enc / 1000.0);
    Serial.println(str_enc);

    #if DEBUG_ENCODER >= 2
        Serial.print("Encoder magnet status: ");
        Serial.println(rotative_encoder.get_magnet_distance());
    #endif

    #if DEBUG_ENCODER >= 3
        char c_enc = 'x';

        stepper_motor.set_direction(HR4988_POSITIVE_DIR);
        stepper_motor.set_speed(60);
        while (c_enc != 'e') {
            if (Serial.available()) {
                c_enc = Serial.read();
            }
            stepper_motor.step();

            angle = rotative_encoder.read_angle();

            sprintf(str_enc, "Encoder reading: %d", angle);
            Serial.println(str_enc);

            delay(250);
        }
    #endif

#endif

#if DEBUG_POTENTIOMETER
    char str_pot[100];
    unsigned long int t_pot;
    uint16_t lin_pos;

    t_pot = micros();
    for (int i=0; i<1000; i++) {
        lin_pos = linear_potentiometer.read_position();
    }
    t_pot = micros() - t_pot;

    sprintf(str_pot, "Linear potentiometer average read time: %f", (float) t_pot / 1000.0);
    Serial.println(str_pot);

    #if DEBUG_POTENTIOMETER >= 2
        char c_pot = 'x';

        while (c_pot != 'e') {
            if (Serial.available()) {
                c_pot = Serial.read();
            }
            lin_pos = linear_potentiometer.raw_read();

            sprintf(str_pot, "Potentiometer reading: %d", lin_pos);
            Serial.println(str_pot);

            delay(250);
        }
    #endif
#endif
}