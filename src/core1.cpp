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


uint8_t limit_reached = 0;
void IRAM_ATTR limit_switch_isr();

button_parameters limit_switch_parameters = {
    LIMIT_SWITCH_PIN, INPUT_PULLUP, LOW, limit_switch_isr, FALLING
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

    button_setup(&limit_switch_parameters);

    button_setup(&shift_up_button_parameters);
    button_setup(&shift_down_button_parameters);

    button_setup(&calibration_button_parameters);

    stepper_motor.set_limit_switch(&limit_reached);
    //stepper_motor.set_rotative_encoder(&rotative_encoder, increase_encoder_direction_sign);
    //stepper_motor.set_linear_potentiometer(&linear_potentiometer, increase_potentiometer_direction_sign);
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
            char c_enc = 'x';

            stepper_motor.set_direction(CCW);
            stepper_motor.set_speed(60);
            while (c_enc != 'e') {
                if (Serial.available()) {
                    c_enc = Serial.read();
                }
                stepper_motor.step();

                angle = rotative_encoder.read_angle();

                sprintf(str_enc, "I2C encoder reading: %d\t\tADC encoder reading: %d", angle, rotative_encoder.read_angle_output());
                Serial.println(str_enc);

                delay(250);
            }
        #endif

        #if DEBUG_ENCODER <= -1
            Serial.print("Encoder magnet status: ");
            Serial.println(rotative_encoder.get_magnet_distance());
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

    shift_down_pressed = shift_up_pressed = calibration_button_pressed = 0;
    while (!(shift_down_pressed || shift_up_pressed || calibration_button_pressed)) delay(10);

    if (shift_up_pressed) {
        int time_long_press = millis();
        while ((shift_up_pressed = button_read_attach_interrupt(&shift_up_button_parameters)));
        time_long_press = millis() - time_long_press;
        
        if (time_long_press >= 3000) {
            stepper_motor.set_direction(NEGATIVE_DIR);
            stepper_motor.set_speed(60);
            while (!limit_reached) {
                stepper_motor.step();
            }
            stepper_motor.set_position(0);
        }
        else {
            stepper_motor.set_position(0);
        }
    }
    else if (shift_down_pressed) {
        while ((shift_down_pressed = button_read_attach_interrupt(&shift_down_button_parameters)));
        test_mode();
    }
    else if (calibration_button_pressed) {
        while ((calibration_button_pressed = button_read_attach_interrupt(&calibration_button_parameters)));
        calibration();
    }

    #if DEBUG_CORES
        Serial.print("The limit switch isr has runned on core: ");
        Serial.println(limit_reached - 1);
    #endif

    if (limit_reached) {
        stepper_motor.change_direction();
        while ((limit_reached = button_read_attach_interrupt(&limit_switch_parameters))) {
            stepper_motor.step();
        }
    }
    
    #if DEBUG_MOTOR >= 2
        stepper_motor.debug_serial_control();
    #endif


    shift_up_pressed = shift_down_pressed = calibration_button_pressed = limit_reached = 0;

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

        if (limit_reached) {
            #if DEBUG_LIMIT_SWITCH
                Serial.println("Limit reached");
            #endif

            stepper_motor.change_direction();
            stepper_motor.set_speed(MIN_MOVE_RPM);
            while ((limit_reached = button_read_attach_interrupt(&limit_switch_parameters))) {
                stepper_motor.step();
            }
        }

        delay(10);

    }
}


void IRAM_ATTR limit_switch_isr() {
    limit_reached = button_interrupt_service_routine(&limit_switch_parameters);
    #if DEBUG_CORES
        if (limit_reached) limit_reached = xPortGetCoreID() + 1;
    #endif
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
    int pos;
    int delta_pos = 10 * stepper_motor.get_delta_position_360_degrees_rotation();
    
    Serial.println("Test mode");

    while (1) {

        if (shift_up_pressed) {
            #if DEBUG_BUTTONS
                Serial.println("Shifting up");
            #endif
            pos = stepper_motor.get_position();
            stepper_motor.move(pos + delta_pos);

            while ((shift_up_pressed = button_read_attach_interrupt(&shift_up_button_parameters)));
        }

        if (shift_down_pressed) {
            #if DEBUG_BUTTONS
                Serial.println("Shifting down");
            #endif
            pos = stepper_motor.get_position();
            stepper_motor.move(pos - delta_pos);

            while ((shift_down_pressed = button_read_attach_interrupt(&shift_down_button_parameters)));
        }

        if (limit_reached) {
            #if DEBUG_LIMIT_SWITCH
                Serial.println("Limit reached");
            #endif

            while((limit_reached = button_read_attach_interrupt(&limit_switch_parameters)));
        }

        delay(10);
    }
}