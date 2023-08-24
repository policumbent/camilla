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


Memory flash = Memory();



void function_core_1 (void *parameters) {

    #if DEBUG_CORES
        Serial.print("Task 1 initialized running on core: ");
        Serial.println(xPortGetCoreID());
    #endif

    delay(1000);

    button_setup(&limit_switch_begin_parameters);
    button_setup(&limit_switch_end_parameters);

    button_setup(&shift_up_button_parameters);
    button_setup(&shift_down_button_parameters);

    button_setup(&calibration_button_parameters);


#if ENCODER_CONNECTED
    stepper_motor.set_rotative_encoder(&rotative_encoder, increase_encoder_direction_sign);
#endif
#if POTENTIOMETER_CONNECTED
    stepper_motor.set_linear_potentiometer(&linear_potentiometer, increase_potentiometer_direction_sign);
#endif

    stepper_motor.set_limit_switch_begin(&switch_begin_pressed, &limit_switch_begin_parameters);
    stepper_motor.set_limit_switch_end(&switch_end_pressed, &limit_switch_end_parameters);
    
    int *gears_ptr = gears;
    stepper_motor.set_gears(gears_ptr);
    int *gears_lin_ptr = gears_lin;
    stepper_motor.set_gears_lin(gears_lin_ptr);

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
    while (!(shift_down_pressed || shift_up_pressed || calibration_button_pressed)) delay(10);

    if (shift_down_pressed) {
        while ((shift_down_pressed = button_read_attach_interrupt(&shift_down_button_parameters)));
        digitalWrite(BUILT_IN_LED_PIN, LOW);

        test_mode();
    }

    if (calibration_button_pressed) {
        while ((calibration_button_pressed = button_read_attach_interrupt(&calibration_button_parameters)));
        digitalWrite(BUILT_IN_LED_PIN, LOW);

        g_calibration_flag = 1;
        calibration();
        while ((calibration_button_pressed = button_read_attach_interrupt(&calibration_button_parameters)));
    }

    digitalWrite(BUILT_IN_LED_PIN, HIGH);

    while (!shift_up_pressed) delay(10);
    while ((shift_up_pressed = button_read_attach_interrupt(&shift_up_button_parameters)));

    digitalWrite(BUILT_IN_LED_PIN, LOW);

    #if DEBUG
        Serial.println("GEARS MODE");
    #endif

    stepper_motor.go_to_limit_switch(FEEDBACKSTEPPER_LIMIT_SWITCH_BEGIN_TYPE);
    stepper_motor.set_position(0);

    #if DEBUG_MOTOR >= 2
        stepper_motor.debug_serial_control();
    #endif


    shift_up_pressed = shift_down_pressed = calibration_button_pressed = 0;
    switch_begin_pressed = switch_end_pressed = 0;

    shift(1);

    while (1) {

        if (shift_up_pressed) {
            #if DEBUG_BUTTONS
                Serial.println("Shifting up");
            #endif
            shift(g_current_gear + 1);

            while ((shift_up_pressed = button_read_attach_interrupt(&shift_up_button_parameters)));
        }

        if (shift_down_pressed) {
            #if DEBUG_BUTTONS
                Serial.println("Shifting down");
            #endif
            shift(g_current_gear - 1);

            while ((shift_down_pressed = button_read_attach_interrupt(&shift_down_button_parameters)));
        }

        if (switch_begin_pressed) {
            #if DEBUG_LIMIT_SWITCH
                Serial.println("Begin limit reached");
            #endif

            stepper_motor.move_while_button_pressed(100, HR4988_CHANGE_DIR, &switch_begin_pressed, &limit_switch_begin_parameters);
        }

        if (switch_end_pressed) {
            #if DEBUG_LIMIT_SWITCH
                Serial.println("End limit reached");
            #endif

            stepper_motor.move_while_button_pressed(100, HR4988_CHANGE_DIR, &switch_end_pressed, &limit_switch_end_parameters);
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
    const int SPEED = 200;
    uint8_t end;

    #if DEBUG
        Serial.println("TEST MODE");
    #endif

    stepper_motor.set_speed(SPEED);

    shift_up_pressed = shift_down_pressed = calibration_button_pressed = 0;

    end = 0;
    while (!end) {

        if (shift_up_pressed) {
            stepper_motor.set_direction(HR4988_POSITIVE_DIR);
            stepper_motor.move_while_button_pressed(SPEED, HR4988_POSITIVE_DIR, &shift_up_pressed, &shift_up_button_parameters);
        }

        if (shift_down_pressed) {
            stepper_motor.set_direction(HR4988_NEGATIVE_DIR);
            stepper_motor.move_while_button_pressed(SPEED, HR4988_NEGATIVE_DIR, &shift_down_pressed, &shift_down_button_parameters);
        }

        if (switch_begin_pressed) {
            stepper_motor.change_direction();
            stepper_motor.move_while_button_pressed(SPEED, HR4988_CHANGE_DIR, &switch_begin_pressed, &limit_switch_begin_parameters);
        }

        if (switch_end_pressed) {
            stepper_motor.change_direction();
            stepper_motor.move_while_button_pressed(SPEED,HR4988_CHANGE_DIR,  &switch_end_pressed, &limit_switch_end_parameters);
        }

        if (calibration_button_pressed) {
            end = 1;
            while ((calibration_button_pressed = button_read_attach_interrupt(&calibration_button_parameters)));
        }

        delay(10);

    }
}


void IRAM_ATTR limit_switch_begin_isr() {
    switch_begin_pressed = button_interrupt_service_routine(&limit_switch_begin_parameters);
}


void IRAM_ATTR limit_switch_end_isr() {
    switch_end_pressed = button_interrupt_service_routine(&limit_switch_end_parameters);
}


void IRAM_ATTR shift_up_button_isr() {
    shift_up_pressed = button_interrupt_service_routine(&shift_up_button_parameters);
}


void IRAM_ATTR shift_down_button_isr() {
    shift_down_pressed = button_interrupt_service_routine(&shift_down_button_parameters);
}


void IRAM_ATTR calibration_button_isr() {
    calibration_button_pressed = button_interrupt_service_routine(&calibration_button_parameters);
}