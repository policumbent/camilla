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


Memory flash = Memory ();


// Measured total length 184976
//  184976 / 11 / 16 = 1051
//  so starting from 0 at the end, 12 gears from 0 to -184976, each distant 1051 * 16
#if GEARS_SETUP
    #if (LIMIT_SWITCH_AS_REFERENCE && ZERO_POSITION_AT_BEGIN) || (!LIMIT_SWITCH_AS_REFERENCE)
        int gears[NUM_GEARS] = {0, 57718, 83914, 104380, 125871, 147923, 171416, 198621};
    #else
        int gears[NUM_GEARS] = {-184976, -168160, -151344, -134528, -117712, -100896, -84080, -67264, -50448, -33632, -16816, 0};
    #endif
    int gears_lin[NUM_GEARS] = {0, 0, 0, 0, 0, 0, 0, 0};
#else
    int gears[NUM_GEARS];
    int gears_lin[NUM_GEARS];
#endif


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


    #if GEARS_SETUP
        flash.write_array(gears_memory_key, (void *) gears, NUM_GEARS, sizeof(int));
        flash.write_array(gears_lin_memory_key, (void *) gears_lin, NUM_GEARS, sizeof(int));
    #endif    

    flash.read_array(gears_memory_key, (void *) gears, NUM_GEARS, sizeof(int));
    flash.read_array(gears_lin_memory_key, (void *) gears_lin, NUM_GEARS, sizeof(int));

#if DEBUG
    debug();
#endif

    blink_built_in_led(4);

#if DEFAULT_MODE == DEFAULT_MODE_GEARS
    gears_mode();
#elif DEFAULT_MODE == DEFAULT_MODE_WHILE_PRESSED
    while_pressed_mode();
#endif


    digitalWrite(BUILTIN_LED, HIGH);

    shift_down_pressed = shift_up_pressed = calibration_button_pressed = 0;
    while (1) {

        if (shift_up_pressed) {
            while ((shift_up_pressed = button_read_attach_interrupt(&shift_up_button_parameters)));
            digitalWrite(BUILTIN_LED, LOW);

            gears_mode();

            g_current_gear = 0;

            digitalWrite(BUILTIN_LED, HIGH);
        }

        if (shift_down_pressed) {
            while ((shift_down_pressed = button_read_attach_interrupt(&shift_down_button_parameters)));
            digitalWrite(BUILTIN_LED, LOW);

            while_pressed_mode();

            digitalWrite(BUILTIN_LED, HIGH);
        }

        if (calibration_button_pressed) {
            while ((calibration_button_pressed = button_read_attach_interrupt(&calibration_button_parameters)));
            digitalWrite(BUILTIN_LED, LOW);

            g_calibration_flag = 1;
            calibration();
            while ((calibration_button_pressed = button_read_attach_interrupt(&calibration_button_parameters)));

            digitalWrite(BUILTIN_LED, HIGH);
        }

        delay(10);
    }

}



// The Nevada gears mode has the overshoot and the possibility of manual shifting
//  this is to compensate the errors in feedback or the absence of it
// The driver can force a manual shifting by keeping the button pressed till the gear changes
// The delta manual position will be added to all gears assuming that the stepper has lost steps
//  and so after manual moving the correct position of the gear is reached, so the relative
//  distance of the following gears does not change

#if NEVADA_MODE

void gears_mode() {
    uint8_t end, manual_moving;
    int elapsed_time, delta_manual;

    #if DEBUG
        Serial.println("GEARS MODE");
    #endif

    initialize_position();

    #if DEBUG_MOTOR >= 2
        stepper_motor.debug_serial_control();
    #endif

    delta_manual = manual_moving = 0;

    shift_up_pressed = shift_down_pressed = calibration_button_pressed = 0;
    switch_begin_pressed = switch_end_pressed = 0;

    shift(1, delta_manual);

    end = 0;
    while (!end) {

        if (shift_up_pressed) {

            elapsed_time = millis();
            while ((shift_up_pressed = button_read_attach_interrupt(&shift_up_button_parameters))) {
                if (millis() - elapsed_time > 1500) {
                    #if DEBUG_BUTTONS
                        Serial.println("Manual shifting up");
                    #endif

                    int current_delta = stepper_motor.get_position();
                    stepper_motor.move_while_button_pressed_check_limit_switches(200, HR4988_POSITIVE_DIR, &shift_up_pressed, &shift_up_button_parameters);
                    current_delta = stepper_motor.get_position() - current_delta;
                    delta_manual += current_delta; 
                    manual_moving = 1;
                }
            }

            if (!manual_moving) {
                #if DEBUG_BUTTONS
                    Serial.println("Shifting up");
                #endif

                shift(g_current_gear + 1, delta_manual);
                stepper_motor.shift_overshoot();
            }

            manual_moving = 0;
        }

        if (shift_down_pressed) {

            elapsed_time = millis();
            while ((shift_down_pressed = button_read_attach_interrupt(&shift_down_button_parameters))) {
                if (millis() - elapsed_time > 1500) {
                    #if DEBUG_BUTTONS
                        Serial.println("Manual shifting up");
                    #endif

                    int current_delta = stepper_motor.get_position();
                    stepper_motor.move_while_button_pressed_check_limit_switches(200, HR4988_NEGATIVE_DIR, &shift_down_pressed, &shift_down_button_parameters);
                    current_delta = stepper_motor.get_position() - current_delta;
                    delta_manual += current_delta; 
                    manual_moving = 1;
                }
            }

            if (!manual_moving) {
                #if DEBUG_BUTTONS
                    Serial.println("Shifting down");
                #endif

                shift(g_current_gear - 1, delta_manual);
                stepper_motor.shift_overshoot();
            }

            manual_moving = 0;
        }

        if (switch_begin_pressed) {
            #if DEBUG_LIMIT_SWITCH
                Serial.println("Begin limit reached");
            #endif

            stepper_motor.move_while_button_pressed(LIMIT_SWITCH_PRESSED_SPEED, HR4988_CHANGE_DIR, &switch_begin_pressed, &limit_switch_begin_parameters, DISTANCE_FROM_LIMIT_SWITCHES);
        }

        if (switch_end_pressed) {
            #if DEBUG_LIMIT_SWITCH
                Serial.println("End limit reached");
            #endif

            stepper_motor.move_while_button_pressed(LIMIT_SWITCH_PRESSED_SPEED, HR4988_CHANGE_DIR, &switch_end_pressed, &limit_switch_end_parameters, DISTANCE_FROM_LIMIT_SWITCHES);
        }

        end = check_long_press(&calibration_button_pressed, &calibration_button_parameters, TIME_BUTTON_LONG_PRESS);

        delay(10);

    }
}


void shift(uint8_t next_gear, int delta_manual) {

    if (next_gear < 1 || next_gear > NUM_GEARS) {
        #if DEBUG_GEARS
            Serial.println("Maximum or minimum gear already reached");
        #endif

        return;
    }

    digitalWrite(BUILTIN_LED, HIGH);

    stepper_motor.move(gears[next_gear-1] + delta_manual);
    g_current_gear = next_gear;

    digitalWrite(BUILTIN_LED, LOW);

    #if DEBUG_GEARS
        Serial.print("Gear: "); Serial.println(g_current_gear);
        Serial.print("Delta manual: "); Serial.println(delta_manual);
        Serial.println();
    #endif
}


#else


void gears_mode() {
    uint8_t end;
    int elapsed_time;

    #if DEBUG
        Serial.println("GEARS MODE");
    #endif

    //stepper_motor.go_to_limit_switch(zero_reference_limit_switch_type);
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

            while ((shift_up_pressed = button_read_attach_interrupt(&shift_up_button_parameters)));

            shift(g_current_gear + 1);
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

            stepper_motor.move_while_button_pressed(LIMIT_SWITCH_PRESSED_SPEED, HR4988_CHANGE_DIR, &switch_begin_pressed, &limit_switch_begin_parameters, DISTANCE_FROM_LIMIT_SWITCHES);
        }

        if (switch_end_pressed) {
            #if DEBUG_LIMIT_SWITCH
                Serial.println("End limit reached");
            #endif

            stepper_motor.move_while_button_pressed(LIMIT_SWITCH_PRESSED_SPEED, HR4988_CHANGE_DIR, &switch_end_pressed, &limit_switch_end_parameters, DISTANCE_FROM_LIMIT_SWITCHES);
        }

        end = check_long_press(&calibration_button_pressed, &calibration_button_parameters, TIME_BUTTON_LONG_PRESS);

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

    digitalWrite(BUILTIN_LED, HIGH);

    stepper_motor.shift(next_gear);
    g_current_gear = next_gear;

    digitalWrite(BUILTIN_LED, LOW);

    #if DEBUG_GEARS
        Serial.print("Gear: "); Serial.println(g_current_gear);
        Serial.println();
    #endif
}

#endif



void while_pressed_mode() {
    uint8_t end;
    int elapsed_time;

    #if DEBUG
        Serial.println("WHILE PRESSED MODE");
    #endif

    shift_up_pressed = shift_down_pressed = calibration_button_pressed = 0;

    end = 0;
    while (!end) {

        move_while_pressed_buttons_control();

        end = check_long_press(&calibration_button_pressed, &calibration_button_parameters, TIME_BUTTON_LONG_PRESS);

        delay(10);

    }
}


void move_while_pressed_buttons_control() {
    if (shift_up_pressed) {
#if LIMIT_SWITCH_BEGIN_CONNECTED || LIMIT_SWITCH_END_CONNECTED
        stepper_motor.move_while_button_pressed_check_limit_switches(HR4988_POSITIVE_DIR, &shift_up_pressed, &shift_up_button_parameters);
#else
        stepper_motor.move_while_button_pressed(HR4988_POSITIVE_DIR, &shift_up_pressed, &shift_up_button_parameters, 0);
        
#endif
    }

    if (shift_down_pressed) {
#if LIMIT_SWITCH_BEGIN_CONNECTED || LIMIT_SWITCH_END_CONNECTED
        stepper_motor.move_while_button_pressed_check_limit_switches(HR4988_NEGATIVE_DIR, &shift_down_pressed, &shift_down_button_parameters);
#else
        stepper_motor.move_while_button_pressed(HR4988_NEGATIVE_DIR, &shift_down_pressed, &shift_down_button_parameters, 0);
        
#endif
    }

    if (switch_begin_pressed) {
        stepper_motor.move_while_button_pressed(LIMIT_SWITCH_PRESSED_SPEED, HR4988_POSITIVE_DIR, &switch_begin_pressed, &limit_switch_begin_parameters, DISTANCE_FROM_LIMIT_SWITCHES);
    }

    if (switch_end_pressed) {
        stepper_motor.move_while_button_pressed(LIMIT_SWITCH_PRESSED_SPEED, HR4988_NEGATIVE_DIR, &switch_end_pressed, &limit_switch_end_parameters, DISTANCE_FROM_LIMIT_SWITCHES);
    }
}



void initialize_position() {

    #if LIMIT_SWITCH_AS_REFERENCE
        stepper_motor.go_to_limit_switch(zero_reference_limit_switch_type);
    #else
        uint8_t end;

        #if DEBUG
            Serial.println("Move manually the gear to first gear, then press the calibration button");
        #endif

        shift_up_pressed = shift_down_pressed = calibration_button_pressed = 0;

        end = 0;
        while (!end) {

            move_while_pressed_buttons_control();

            if (calibration_button_pressed) {
                while(calibration_button_pressed = button_read_attach_interrupt(&calibration_button_parameters));
                end = 1;
            }

            delay(10);

        }
    #endif

    stepper_motor.set_position(0);
}