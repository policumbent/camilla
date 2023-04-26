#include "core1.h"


HR4988 stepper_motor = HR4988 (
    STEP_PIN, DIRECTION_PIN,
    MS1_PIN, MS2_PIN, MS3_PIN,
    ENABLE_PIN,
    full_steps_per_turn, deg_per_full_step,
    cw_direction_sign
);


AS5600 rotative_encoder = AS5600 (
    MAGNETIC_ENCODER_PIN
);


int gears[NUM_GEARS];


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

    #if DEBUG_MEMORY >= 2
        for (int i=0; i<NUM_GEARS; i++)
            gears[i] = 4 * stepper_motor.get_delta_position_turn() * (i+1);
        flash.write_array(gears_memory_key, (void *) gears, NUM_GEARS, sizeof(int));
        for (int i=0; i<NUM_GEARS; i++)
            gears[i] = 0;
    #endif    

    flash.read_array(gears_memory_key, (void *) gears, NUM_GEARS, sizeof(int));

    #if DEBUG_MEMORY
        Serial.println("Gears read from memory");
        char str_mem[50];
        for (int i=0; i<NUM_GEARS; i++) {
            sprintf(str_mem, "Gear: %d \tPosition: %d", i+1, gears[i]);
            Serial.println(str_mem);
        }
    #endif

    #if DEBUG_MOTOR
        stepper_motor.set_speed(60);
        
        char str_mot[100];
        unsigned long int time;
        
        for (int i=0; i<10; i++) {
            time = micros();
            stepper_motor.step();
            time = micros() - time;
            
            sprintf(str_mot, "Measured delay per step: %ld\tExpected delay per step: %ld",
                time, stepper_motor.get_expected_step_time());
            Serial.println(str_mot);
        }

        time = micros();
        for (int i=0; i<10000; i++) {
            stepper_motor.step();
        }
        time = micros() - time;
        sprintf(str_mot, "Measured delay per step: %lf\tExpected delay per step: %ld",
            (double) time / 10000.0, stepper_motor.get_expected_step_time());
        Serial.println(str_mot);
    #endif

    #if DEBUG_ENCODER
        char str_enc[100];
        unsigned long int t;
        int angle;

        t = micros();
        for (int i=0; i<1000; i++) {
            angle = rotative_encoder.read_angle();
        }
        t = micros() - t;

        sprintf(str_enc, "Average read time: %f\t", (float) t / 1000.0);
        Serial.println(str_enc);
    #endif


    while (!shift_down_pressed) delay(1);
    int time_long_press = millis();
    while ((shift_down_pressed = button_read_attach_interrupt(&shift_down_button_parameters)));
    time_long_press = millis() - time_long_press;
    if (time_long_press >= 3000) {
        calibration();
    }

    stepper_motor.set_direction((cw_direction_sign == 1) ? CCW : CW);
    stepper_motor.set_speed(60);
    while (!limit_reached) {
        stepper_motor.step();
    }
    stepper_motor.set_position(0);

    #if DEBUG_CORES
        Serial.print("The limit switch isr has runned on core: ");
        Serial.println(limit_reached - 1);
    #endif

    stepper_motor.change_direction();
    while ((limit_reached = button_read_attach_interrupt(&limit_switch_parameters))) {
        stepper_motor.step();
    }
    
    shift(1);
    

    #if DEBUG_MOTOR >= 2
        stepper_motor.debug_serial_control();
    #endif


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

            while((limit_reached = button_read_attach_interrupt(&limit_switch_parameters)));
        }

        
        // TODO: signal to core0 the need to pass the gear to CAN-BUS

    }
}


void IRAM_ATTR limit_switch_isr() {
    limit_reached = button_interrupt_service_routine(&limit_switch_parameters);
    #if DEBUG_CORES
        if (limit_reached)
            limit_reached = xPortGetCoreID() + 1;
    #endif
}


void IRAM_ATTR shift_up_button_isr() {
    shift_up_pressed = button_interrupt_service_routine(&shift_up_button_parameters);
}


void IRAM_ATTR shift_down_button_isr() {
    shift_down_pressed = button_interrupt_service_routine(&shift_down_button_parameters);
}


void shift(uint8_t next_gear) {

    if (next_gear < 1 || next_gear > NUM_GEARS) {
        #if DEBUG_GEARS
            Serial.println("Maximum or minimum gear already reached");
        #endif

        return;
    }

    int start_pos, target_pos;
    start_pos = stepper_motor.get_position();
    target_pos = gears[next_gear-1];

    stepper_motor.move(start_pos, target_pos, rotative_encoder, &limit_reached);

    g_current_gear = next_gear;

    #if DEBUG_GEARS
        Serial.print("Gear: "); Serial.println(g_current_gear);
    #endif
}