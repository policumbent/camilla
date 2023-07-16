#include "FeedbackStepper.h"


FeedbackStepper :: FeedbackStepper (uint8_t step_pin, uint8_t direction_pin,
                                    uint8_t ms1_pin, uint8_t ms2_pin, uint8_t ms3_pin,
                                    uint8_t enable_pin, uint8_t sleep_pin, uint8_t reset_pin,
                                    int full_steps_per_turn, float deg_per_full_step,
                                    int8_t cw_direction_sign)
: HR4988 (step_pin, direction_pin,
          ms1_pin, ms2_pin, ms3_pin,
          enable_pin, sleep_pin, reset_pin,
          full_steps_per_turn, deg_per_full_step,
          cw_direction_sign)
{
    setup();
}


FeedbackStepper :: FeedbackStepper (uint8_t step_pin, uint8_t direction_pin,
                                    uint8_t ms1_pin, uint8_t ms2_pin, uint8_t ms3_pin,
                                    uint8_t enable_pin,
                                    int full_steps_per_turn, float deg_per_full_step,
                                    int8_t cw_direction_sign)
: HR4988 (step_pin, direction_pin,
          ms1_pin, ms2_pin, ms3_pin,
          enable_pin,
          full_steps_per_turn, deg_per_full_step,
          cw_direction_sign)
{
    setup();
}


FeedbackStepper :: FeedbackStepper (uint8_t step_pin, uint8_t direction_pin,
                                    uint8_t enable_pin,
                                    int full_steps_per_turn, float deg_per_full_step,
                                    int8_t cw_direction_sign)
: HR4988 (step_pin, direction_pin,
          enable_pin,
          full_steps_per_turn, deg_per_full_step,
          cw_direction_sign)
{
    setup();
}


void FeedbackStepper :: setup() {
    rotative_encoder = NULL;
    linear_potentiometer = NULL;
    limit_reached = NULL;
    gears = NULL;
    gears_lin = NULL;
    increase_potentiometer_direction_sign = 0;
    increase_encoder_direction_sign = 0;
}


void FeedbackStepper :: set_rotative_encoder(AS5600 *rotative_encoder, int8_t increase_encoder_direction_sign) {
    this->rotative_encoder = rotative_encoder;
    this->increase_encoder_direction_sign = increase_encoder_direction_sign;
}


void FeedbackStepper :: set_linear_potentiometer(Potentiometer *linear_potentiometer, int8_t increase_potentiometer_direction_sign) {
    this->linear_potentiometer = linear_potentiometer;
    this->increase_potentiometer_direction_sign = increase_potentiometer_direction_sign;
}


void FeedbackStepper :: set_limit_switch(uint8_t *limit_reached) {
    this->limit_reached = limit_reached;
}


void FeedbackStepper :: set_gears(int *gears) {
    this->gears = gears;
}


void FeedbackStepper :: set_gears_lin(int *gears_lin) {
    this->gears_lin = gears_lin;
}


void FeedbackStepper :: move(int target_pos) {
    HR4988::move(target_pos);
}


void FeedbackStepper :: shift(int next_gear) {
    int target_pos = gears[next_gear-1];
    int start_pos = position_sixteenth;

    long int elapsed_time, delay;

    int delta_pos, delta_angle, read_angle, error;
    
    #if DEBUG_FEEDBACK_STEPPER
        Serial.print("[FeedbackStepper] Shift from "); Serial.print(start_pos); Serial.print(" to "); Serial.println(target_pos);
        long int debug_t = micros();
        int expected_delay = 0, tot_angle = 0, step_cnt = 0, avg_error = 0, read_cnt = 0, tot_correction = 0;
    #endif

    #if DEBUG_FEEDBACK_STEPPER >= 2
        int ARRAY_SIZE = 3000;
        int16_t delta_pos_array[ARRAY_SIZE], delta_angle_array[ARRAY_SIZE], error_array[ARRAY_SIZE];
        int array_pos = 0;
    #endif

    // If the limit switch is not connected create a dummy variable to have the limit reached condition never triggered
    uint8_t dummy_limit_reached = 0;
    uint8_t *ptr_limit_reached = limit_reached;
    if (ptr_limit_reached == NULL) {
        ptr_limit_reached = &dummy_limit_reached;
    }
    
    delta_pos = 0;
    delta_angle = 0;
    error = 0;

    if (rotative_encoder != NULL) rotative_encoder->read_angle();

    while (position_sixteenth != target_pos && !(*ptr_limit_reached)) {

        portDISABLE_INTERRUPTS();
        elapsed_time = micros();
        portENABLE_INTERRUPTS();

        _move_set_speed_direction(start_pos, target_pos);

        _step_no_delay_off();


        if (rotative_encoder != NULL && abs(delta_pos) >= 2 * 16) {
            delta_angle = rotative_encoder->get_angle();
            read_angle = rotative_encoder->read_angle();        // USES INTERRUPTS (!!!!)

            // Calculate the difference between previous and current angle measurements
            if (abs(read_angle - delta_angle) < 3000) {
                delta_angle = read_angle - delta_angle;
                delta_angle = (increase_encoder_direction_sign == 1) ? (delta_angle) : (- delta_angle);
            } else {
                if (read_angle > delta_angle) {
                    delta_angle = 4095 - read_angle + delta_angle;
                } else {
                    delta_angle = 4095 - delta_angle + read_angle;
                }
                delta_angle = (direction == POSITIVE_DIR && increase_encoder_direction_sign == 1
                                || direction == NEGATIVE_DIR && increase_encoder_direction_sign == -1)
                                ? (delta_angle) : (- delta_angle);
            }
            
            // Cannot use 'continue;' statement (buggy behavior) (!!!!)

            // Remove faulty readings (see spikes in encoder_reasings.py in docs)
            if (1 || abs(delta_angle) < 150) {
                
                error = delta_pos - ((float) delta_angle) * 0.7814;      // angle / 4095 * 200 * 16

                
                if (abs(delta_angle) < 150 && error >= 8) {
                    error = round((float) error / (float) microstepping) * microstepping;
                    position_sixteenth -= error;
                }
                
                // If motor is blocked, restart acceleration
                if (abs(delta_angle) <= 16) {
                    start_pos = position_sixteenth;
                }
                
            
                #if DEBUG_FEEDBACK_STEPPER >= 2
                    delta_pos_array[array_pos] = delta_pos;
                    delta_angle_array[array_pos] = delta_angle;
                    error_array[array_pos] = error;
                    array_pos++;
                #endif

                #if DEBUG_FEEDBACK_STEPPER
                    tot_angle += delta_angle;
                    avg_error += error;
                    read_cnt++;
                    if (error >= 8) tot_correction += error;
                #endif
            }

            delta_pos = 0;
        }
        

        portDISABLE_INTERRUPTS();
        elapsed_time = micros() - elapsed_time;
        delay = (delay_off - elapsed_time > 0) ? (delay_off - elapsed_time) : (1);
        portENABLE_INTERRUPTS();
        //Serial.println(elapsed_time);

        delayMicroseconds(delay);

        delta_pos += _update_position();

        #if DEBUG_FEEDBACK_STEPPER
            expected_delay += get_expected_step_time();
            step_cnt++;
        #endif
    }

    #if DEBUG_FEEDBACK_STEPPER
        debug_t = micros() - debug_t;
        if (step_cnt == 0) return;
        Serial.print("Expected (avg) delay: "); Serial.print(expected_delay / step_cnt);
        Serial.print("\tMeasured (avg) delay: "); Serial.println((double) debug_t / (double) step_cnt);
        Serial.print("Encoder reading: "); Serial.print(tot_angle);
        Serial.print("\t\tAverage error: "); Serial.println((double) avg_error / (double) read_cnt);
        Serial.print("Total correction: "); Serial.println(tot_correction);
    #endif

    #if DEBUG_FEEDBACK_STEPPER >= 2
        for (int i=0; i<array_pos; i++) {
            Serial.print(delta_pos_array[i]); Serial.print("\t");
            Serial.print(delta_angle_array[i]); Serial.print("\t");
            Serial.print(error_array[i]); Serial.print("\n");
        }
    #endif


    // If the linear position is not correct, correct the shift
    uint16_t ACCEPTABLE_ERROR = 5;
    uint16_t pot_read;
    int8_t dir = 2;

    set_speed(MIN_MOVE_RPM);

    
    while (linear_potentiometer != NULL && dir != 0 && !(*ptr_limit_reached)) {

        portDISABLE_INTERRUPTS();
        elapsed_time = micros();
        portENABLE_INTERRUPTS();

        pot_read = linear_potentiometer->read_position();

        if (pot_read < gears_lin[next_gear-1] - ACCEPTABLE_ERROR) {
            dir = (increase_potentiometer_direction_sign == 1) ? POSITIVE_DIR : NEGATIVE_DIR;
        } else if (pot_read > gears_lin[next_gear-1] + ACCEPTABLE_ERROR) {
            dir = (increase_potentiometer_direction_sign == 1) ? NEGATIVE_DIR : POSITIVE_DIR;
        }

        if (dir == POSITIVE_DIR || dir == NEGATIVE_DIR) {
            set_direction(dir);

            _step_no_delay_off();
            _update_position();

            portDISABLE_INTERRUPTS();
            elapsed_time = micros() - elapsed_time;
            delay = (delay_off - elapsed_time > 0) ? (delay_off - elapsed_time) : (1);
            portENABLE_INTERRUPTS();
            //Serial.println(elapsed_time);
        }
        else {
            dir = 0;
        }
    }
}