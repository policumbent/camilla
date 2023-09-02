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
    gears = NULL;
    gears_lin = NULL;
    increase_potentiometer_direction_sign = 0;
    increase_encoder_direction_sign = 0;

    // If the limit switch is not connected use a dummy variable to have the limit reached condition never triggered
    dummy_limit_reached = 0;
    limit_begin_reached = &dummy_limit_reached;
    limit_end_reached = &dummy_limit_reached;
}


void FeedbackStepper :: set_rotative_encoder(AS5600 *rotative_encoder, int8_t increase_encoder_direction_sign) {
    this->rotative_encoder = rotative_encoder;
    this->increase_encoder_direction_sign = increase_encoder_direction_sign;
}


void FeedbackStepper :: set_linear_potentiometer(Potentiometer *linear_potentiometer, int8_t increase_potentiometer_direction_sign) {
    this->linear_potentiometer = linear_potentiometer;
    this->increase_potentiometer_direction_sign = increase_potentiometer_direction_sign;
}


void FeedbackStepper :: set_limit_switch_begin(uint8_t *limit_begin_reached, button_parameters *switch_begin_parameters) {
    this->limit_begin_reached = limit_begin_reached;
    this->switch_begin_paramters = switch_begin_parameters;
}


void FeedbackStepper :: set_limit_switch_end(uint8_t *limit_end_reached, button_parameters *switch_end_parameters) {
    this->limit_end_reached = limit_end_reached;
    this->switch_end_parameters = switch_end_parameters;
}


void FeedbackStepper :: set_gears(int *gears) {
    this->gears = gears;
}


void FeedbackStepper :: set_gears_lin(int *gears_lin) {
    this->gears_lin = gears_lin;
}


void FeedbackStepper :: shift(int next_gear) {
    
    move(gears[next_gear-1]);

    // If the linear position is not correct, correct the shift
    if (linear_potentiometer != NULL) {
        _shift_linear_correction(next_gear);
    }
}


void FeedbackStepper :: move(int target_pos) {
    int start_pos;
    long int elapsed_time, delay;
    int delta_pos, prev_angle, delta_angle, read_angle, error;
    bool faulty_reading;
    
    #if FEEDBACKSTEPPER_DEBUG
        Serial.print("[FeedbackStepper] Shift from "); Serial.print(start_pos); Serial.print(" to "); Serial.println(target_pos);
        long int debug_t = micros();
        int expected_delay = 0, tot_angle = 0, step_cnt = 0, avg_error = 0, read_cnt = 0, tot_correction = 0;
    #endif

    #if FEEDBACKSTEPPER_DEBUG >= 2
        int ARRAY_SIZE = 3000;
        int16_t delta_pos_array[ARRAY_SIZE], delta_angle_array[ARRAY_SIZE], error_array[ARRAY_SIZE];
        int array_pos = 0;
    #endif
    
    delta_pos = 0;
    delta_angle = 0;
    error = 0;
    faulty_reading = false;

    start_pos = position_sixteenth;

    if (rotative_encoder != NULL) {
        prev_angle = rotative_encoder->read_angle();
    }

    while (position_sixteenth != target_pos && !(*limit_begin_reached) && !(*limit_end_reached)) {

        portDISABLE_INTERRUPTS();
        elapsed_time = micros();
        portENABLE_INTERRUPTS();

        _move_set_speed_direction(start_pos, target_pos);

        _step_no_delay_off();

        // The correction of the position with the encoder is not done in a proper function since the code
        //  in the debug code in the #if statements cannot be easily done in another function

        // Correct with the encoder after a proper delta_pos or if a faulty reading was detected
        if (rotative_encoder != NULL && abs(delta_pos) >= 4 * 16) {

            // If previous reading is faulty, do not take angle with get_angle, since the faulty reading will be
            //  retrieved, but instead keep the previous one
            if (!faulty_reading) {
                prev_angle = rotative_encoder->get_angle();
            }
            else {
                faulty_reading = false;
            }
            
            read_angle = rotative_encoder->read_angle();        // USES INTERRUPTS (!!!!)

            // Calculate the difference between previous and current angle measurements
            if (abs(read_angle - prev_angle) < 3000) {
                delta_angle = read_angle - prev_angle;
                delta_angle = (increase_encoder_direction_sign == 1) ? (delta_angle) : (- delta_angle);
            } else {
                if (read_angle > prev_angle) {
                    delta_angle = 4095 - read_angle + prev_angle;
                } else {
                    delta_angle = 4095 - prev_angle + read_angle;
                }
                delta_angle = (direction == HR4988_POSITIVE_DIR) ? (delta_angle) : (- delta_angle);
            }
            
            // Cannot use 'continue;' statement (buggy behavior) (!!!!)

            // Remove faulty readings (see spikes in encoder_reasings.py in docs)
            if (abs(delta_angle) > 200) {
                faulty_reading = true;
            }
            else {
                error = delta_pos - ((float) delta_angle) * 0.7814;      // angle / 4095 * 200 * 16

                if (error >= 8) {
                    error = round((float) error / (float) microstepping) * microstepping;
                    position_sixteenth += (direction == HR4988_POSITIVE_DIR) ? (- error) : (error);
                }
                
                // If motor is blocked, restart acceleration
                if (abs(delta_angle) <= 16) {
                    start_pos = position_sixteenth;
                }
                
            
                #if FEEDBACKSTEPPER_DEBUG
                    tot_angle += delta_angle;
                    avg_error += error;
                    read_cnt++;
                    if (error >= 8) tot_correction += error;
                #endif

                #if FEEDBACKSTEPPER_DEBUG >= 2
                    if (array_pos < ARRAY_SIZE) {
                        delta_pos_array[array_pos] = delta_pos;
                        delta_angle_array[array_pos] = delta_angle;
                        error_array[array_pos] = error;
                        array_pos++;
                    }
                #endif

                delta_pos = 0;
            }
        }
        

        portDISABLE_INTERRUPTS();
        elapsed_time = micros() - elapsed_time;
        delay = (delay_off - elapsed_time > 0) ? (delay_off - elapsed_time) : (1);
        portENABLE_INTERRUPTS();
        //Serial.println(elapsed_time);

        delayMicroseconds(delay);

        delta_pos += _update_position();

        #if FEEDBACKSTEPPER_DEBUG
            expected_delay += get_expected_step_time();
            step_cnt++;
        #endif
    }

    #if FEEDBACKSTEPPER_DEBUG
        debug_t = micros() - debug_t;
        if (step_cnt == 0) return;
        Serial.print("Expected (avg) delay: "); Serial.print(expected_delay / step_cnt);
        Serial.print("\tMeasured (avg) delay: "); Serial.println((double) debug_t / (double) step_cnt);
        Serial.print("Encoder reading: "); Serial.print(tot_angle);
        Serial.print("\t\tAverage error: "); Serial.println((double) avg_error / (double) read_cnt);
        Serial.print("Total correction: "); Serial.println(tot_correction);
    #endif

    #if FEEDBACKSTEPPER_DEBUG >= 2
        for (int i=0; i<array_pos; i++) {
            Serial.print(delta_pos_array[i]); Serial.print("  ");
            Serial.print(delta_angle_array[i]); Serial.print("  ");
            Serial.print(error_array[i]); Serial.print("\n");
        }
    #endif
}


void FeedbackStepper :: shift_overshoot() {

    move(position_sixteenth + direction * FEEDBACKSTEPPER_SHIFT_OVERSHOOT_STEPS);

    change_direction();
    delay(FEEDBACKSTEPPER_SHIFT_OVERSHOOT_DELAY);

    move(position_sixteenth + direction * FEEDBACKSTEPPER_SHIFT_OVERSHOOT_STEPS);
}


void FeedbackStepper :: _shift_linear_correction(int next_gear) {
    uint16_t pot_read;
    int8_t dir = 2;
    long int elapsed_time, delay;

    set_speed(HR4988_MIN_MOVE_RPM);
    
    while (dir != 0 && !(*limit_begin_reached) && !(*limit_end_reached)) {

        portDISABLE_INTERRUPTS();
        elapsed_time = micros();
        portENABLE_INTERRUPTS();

        pot_read = linear_potentiometer->read_position();

        if (pot_read < gears_lin[next_gear-1] - FEEDBACKSTEPPER_SHIFT_LINEAR_CORRECTION_ACCEPTABLE_ERROR) {
            dir = (increase_potentiometer_direction_sign == 1) ? HR4988_POSITIVE_DIR : HR4988_NEGATIVE_DIR;
        } else if (pot_read > gears_lin[next_gear-1] + FEEDBACKSTEPPER_SHIFT_LINEAR_CORRECTION_ACCEPTABLE_ERROR) {
            dir = (increase_potentiometer_direction_sign == 1) ? HR4988_NEGATIVE_DIR : HR4988_POSITIVE_DIR;
        }

        if (dir == HR4988_POSITIVE_DIR || dir == HR4988_NEGATIVE_DIR) {
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


void FeedbackStepper :: move_while_button_pressed(int8_t dir, uint8_t *button_pressed, button_parameters *bp) {
    // -1 as speed is used to have acceleration till MAX_SPEED
    move_while_button_pressed(-1, dir, button_pressed, bp);
}


void FeedbackStepper :: move_while_button_pressed(float speed, int8_t dir, uint8_t *button_pressed, button_parameters *bp) {
    uint8_t end = 0;
    long int elapsed_time, delay;

    set_direction(dir);

    int start_pos = position_sixteenth;
    int dummy_target_pos = (dir == HR4988_POSITIVE_DIR) ? INT_MAX : INT_MIN;

    if (speed != -1) {
        set_speed(speed);
    }

    while (!end) {

        portDISABLE_INTERRUPTS();
        elapsed_time = micros();

        *button_pressed = button_read_attach_interrupt(bp);

        if (!(*button_pressed)) {
            end = 1;
            portENABLE_INTERRUPTS();
        }
        else {
            if (speed == -1) {
                _move_set_speed_direction(start_pos, dummy_target_pos);
            }

            _step_no_delay_off();

            elapsed_time = micros() - elapsed_time;
            delay = (delay_off - elapsed_time > 0) ? (delay_off - elapsed_time) : (1);
            portENABLE_INTERRUPTS();

            delayMicroseconds(delay);

            _update_position();
        }
    }    
}


void FeedbackStepper :: go_to_limit_switch(uint8_t limit_switch_type) {

    switch (limit_switch_type) {

        case FEEDBACKSTEPPER_LIMIT_SWITCH_BEGIN_TYPE:

            if (limit_begin_reached == NULL) break;

            set_direction(HR4988_NEGATIVE_DIR);
            set_speed(FEEDBACKSTEPPER_GO_TO_LIMIT_SWITCH_SPEED);
            while (!(*limit_begin_reached)) step();

            move_while_button_pressed(FEEDBACKSTEPPER_GO_TO_LIMIT_SWITCH_SPEED, HR4988_CHANGE_DIR, limit_begin_reached, switch_begin_paramters);

            break;

        case FEEDBACKSTEPPER_LIMIT_SWITCH_END_TYPE:

            if (limit_end_reached == NULL) break;

            set_direction(HR4988_POSITIVE_DIR);
            set_speed(FEEDBACKSTEPPER_GO_TO_LIMIT_SWITCH_SPEED);
            while (!(*limit_end_reached)) step();

            move_while_button_pressed(FEEDBACKSTEPPER_GO_TO_LIMIT_SWITCH_SPEED, HR4988_CHANGE_DIR, limit_end_reached, switch_end_parameters);

            break;

        default:
            break;
    }
}