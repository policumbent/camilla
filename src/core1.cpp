#include "core1.h"


FeedbackStepper stepper_motor = FeedbackStepper (
    STEP_PIN, DIRECTION_PIN,
    MS1_PIN, MS2_PIN, MS3_PIN,
    ENABLE_PIN, SLEEP_PIN, RESET_PIN,
    full_steps_per_turn, deg_per_full_step,
    cw_direction_sign
);


AS5600 rotative_encoder = AS5600 ();


Potentiometer linear_potentiometer = Potentiometer (
    POTENTIOMETER_ADC_CHANNEL
);


int gears[NUM_GEARS];
int gears_lin[NUM_GEARS];


Memory flash = Memory ();


uint8_t zero_reference_limit_switch_type;



void function_core_1 (void *parameters) {

    #if DEBUG_CORES
        Serial.print("Task 1 initialized running on core: ");
        Serial.println(xPortGetCoreID());
    #endif

    delay(1000);
    


#if !MICROSTEPPING_ENABLED
    stepper_motor.disable_microstepping();
#endif

#if ENCODER_CONNECTED
    stepper_motor.set_rotative_encoder(&rotative_encoder, increase_encoder_direction_sign);
#endif
#if POTENTIOMETER_CONNECTED
    stepper_motor.set_linear_potentiometer(&linear_potentiometer, increase_potentiometer_direction_sign);
#endif

#if LIMIT_SWITCH_BEGIN_CONNECTED
    stepper_motor.set_limit_switch_begin(&switch_begin_pressed, &limit_switch_begin_parameters);
#endif
#if LIMIT_SWITCH_END_CONNECTED
    stepper_motor.set_limit_switch_end(&switch_end_pressed, &limit_switch_end_parameters);
#endif
    
    int *gears_ptr = gears;
    stepper_motor.set_gears(gears_ptr);
    int *gears_lin_ptr = gears_lin;
    stepper_motor.set_gears_lin(gears_lin_ptr);

#if ZERO_POSITION_AT_BEGIN
    zero_reference_limit_switch_type = FEEDBACKSTEPPER_LIMIT_SWITCH_BEGIN_TYPE;
#else
    zero_reference_limit_switch_type = FEEDBACKSTEPPER_LIMIT_SWITCH_END_TYPE;
#endif


    #if DEBUG_MEMORY >= 2
        for (int i=0; i<NUM_GEARS; i++) {
            gears[i] = 4 * stepper_motor.get_delta_position_360_degrees_rotation() * (i+1);
            gears_lin[i] = 0;
        }
        flash.write_array(gears_memory_key, (void *) gears, NUM_GEARS, sizeof(int));
        flash.write_array(gears_lin_memory_key, (void *) gears_lin, NUM_GEARS, sizeof(int));
        for (int i=0; i<NUM_GEARS; i++)
            gears[i] = 0;
    #endif    

    flash.read_array(gears_memory_key, (void *) gears, NUM_GEARS, sizeof(int));
    flash.read_array(gears_lin_memory_key, (void *) gears_lin, NUM_GEARS, sizeof(int));

    #if DEBUG_MEMORY
        Serial.println("Gears read from memory");
        char str_mem[50];
        for (int i=0; i<NUM_GEARS; i++) {
            sprintf(str_mem, "Gear: %d \tPosition: %d \tLinear position: %d", i+1, gears[i], gears_lin[i]);
            Serial.println(str_mem);
        }
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


    blink_built_in_led(2);

    digitalWrite(BUILT_IN_LED_PIN, HIGH);

    shift_down_pressed = shift_up_pressed = calibration_button_pressed = 0;
    while (1) {

        if (shift_up_pressed) {
            while ((shift_up_pressed = button_read_attach_interrupt(&shift_up_button_parameters)));
            digitalWrite(BUILT_IN_LED_PIN, LOW);

            gears_mode();

            g_current_gear = 0;

            digitalWrite(BUILT_IN_LED_PIN, HIGH);
        }

        if (shift_down_pressed) {
            while ((shift_down_pressed = button_read_attach_interrupt(&shift_down_button_parameters)));
            digitalWrite(BUILT_IN_LED_PIN, LOW);

            test_mode();

            digitalWrite(BUILT_IN_LED_PIN, HIGH);
        }

        if (calibration_button_pressed) {
            while ((calibration_button_pressed = button_read_attach_interrupt(&calibration_button_parameters)));
            digitalWrite(BUILT_IN_LED_PIN, LOW);

            g_calibration_flag = 1;
            calibration();
            while ((calibration_button_pressed = button_read_attach_interrupt(&calibration_button_parameters)));

            digitalWrite(BUILT_IN_LED_PIN, HIGH);
        }

        delay(10);
    }

}


void gears_mode() {
    uint8_t end, overshoot_flag;
    int elapsed_time;

    #if DEBUG
        Serial.println("GEARS MODE");
    #endif

    stepper_motor.go_to_limit_switch(zero_reference_limit_switch_type);
    stepper_motor.set_position(0);

    #if DEBUG_MOTOR >= 2
        stepper_motor.debug_serial_control();
    #endif


    shift_up_pressed = shift_down_pressed = calibration_button_pressed = 0;
    switch_begin_pressed = switch_end_pressed = 0;

    shift(1);

    end = 0;
    while (!end) {

        if (shift_up_pressed) {
            #if DEBUG_BUTTONS
                Serial.println("Shifting up");
            #endif

            while ((shift_up_pressed = button_read_attach_interrupt(&shift_up_button_parameters))) {
                if (shift_down_pressed) {
                    while ((shift_down_pressed = button_read_attach_interrupt(&shift_down_button_parameters)));
                    stepper_motor.shift_overshoot();
                    overshoot_flag = 1;
                }
            }

            if (!overshoot_flag) {
                shift(g_current_gear + 1);
            }

            overshoot_flag = 0;
        }

        if (shift_down_pressed) {
            #if DEBUG_BUTTONS
                Serial.println("Shifting down");
            #endif

            while ((shift_down_pressed = button_read_attach_interrupt(&shift_down_button_parameters)));

            shift(g_current_gear - 1);
        }

        if (switch_begin_pressed) {
            #if DEBUG_LIMIT_SWITCH
                Serial.println("Begin limit reached");
            #endif

            stepper_motor.move_while_button_pressed(LIMIT_SWITCH_PRESSED_SPEED, HR4988_CHANGE_DIR, &switch_begin_pressed, &limit_switch_begin_parameters);
        }

        if (switch_end_pressed) {
            #if DEBUG_LIMIT_SWITCH
                Serial.println("End limit reached");
            #endif

            stepper_motor.move_while_button_pressed(LIMIT_SWITCH_PRESSED_SPEED, HR4988_CHANGE_DIR, &switch_end_pressed, &limit_switch_end_parameters);
        }

        if (calibration_button_pressed) {
            elapsed_time = millis();
            while ((calibration_button_pressed = button_read_attach_interrupt(&calibration_button_parameters)));
            elapsed_time = millis() - elapsed_time;

            if (elapsed_time > 3000) {
                end = 1;
            }
        }

        delay(10);

    }
}


void shift(uint8_t next_gear) {

    if (next_gear < 1 || next_gear > NUM_GEARS) {
        #if DEBUG_GEARS
            Serial.println("Maximum or minimum gear already reached");
        #endif

        return;
    }

    stepper_motor.shift(next_gear);

    g_current_gear = next_gear;

    #if DEBUG_GEARS
        Serial.print("Gear: "); Serial.println(g_current_gear);
    #endif
}


void test_mode() {
    uint8_t end;

    #if DEBUG
        Serial.println("TEST MODE");
    #endif

    shift_up_pressed = shift_down_pressed = calibration_button_pressed = 0;

    end = 0;
    while (!end) {

        if (shift_up_pressed) {
            stepper_motor.move_while_button_pressed(HR4988_POSITIVE_DIR, &shift_up_pressed, &shift_up_button_parameters);
        }

        if (shift_down_pressed) {
            stepper_motor.move_while_button_pressed(HR4988_NEGATIVE_DIR, &shift_down_pressed, &shift_down_button_parameters);
        }

        if (switch_begin_pressed) {
            stepper_motor.move_while_button_pressed(HR4988_CHANGE_DIR, &switch_begin_pressed, &limit_switch_begin_parameters);
        }

        if (switch_end_pressed) {
            stepper_motor.move_while_button_pressed(HR4988_CHANGE_DIR,  &switch_end_pressed, &limit_switch_end_parameters);
        }

        if (calibration_button_pressed) {
            while ((calibration_button_pressed = button_read_attach_interrupt(&calibration_button_parameters)));
            end = 1;
        }

        delay(10);

    }
}