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
}


void FeedbackStepper :: set_rotative_encoder(AS5600 *rotative_encoder) {
    this->rotative_encoder = rotative_encoder;
}


void FeedbackStepper :: set_linear_potentiometer(Potentiometer *linear_potentiometer) {
    this->linear_potentiometer = linear_potentiometer;
}


void FeedbackStepper :: set_limit_switch(uint8_t *limit_reached) {
    this->limit_reached = limit_reached;
}


void FeedbackStepper :: move(int start_pos, int target_pos) {
    long int elapsed_time, delay;
    int step_cnt;
    uint16_t delta_angle, delta_linear;
    
    #if DEBUG_FEEDBACK_STEPPER
        Serial.print("[FeedbackStepper] Shift from "); Serial.print(start_pos); Serial.print(" to "); Serial.println(target_pos);
        long int debug_t = micros();
        int expected_delay = 0, tot_angle = 0, tot_linear = 0;
    #endif

    // If the limit switch is not connected create a dummy variable to have the limit reached condition never triggered
    uint8_t dummy_limit_reached = 0;
    uint8_t *ptr_limit_reached = limit_reached;
    if (ptr_limit_reached == NULL) {
        ptr_limit_reached = &dummy_limit_reached;
    }
    
    delta_angle = delta_linear = 0;
    step_cnt = 0;

    while (position_sixteenth != target_pos && !(*ptr_limit_reached)) {

        portDISABLE_INTERRUPTS();
        elapsed_time = micros();

        _move_set_speed_direction(start_pos, target_pos);

        _step_no_delay_off();

        if (rotative_encoder != NULL && step_cnt % 2 == 0) {
            delta_angle = (*rotative_encoder).get_angle();
            delta_angle = (*rotative_encoder).read_angle() - delta_angle;
        }
        else if (linear_potentiometer != NULL && step_cnt % 5 == 0) {
            delta_linear = (*linear_potentiometer).get_position();
            delta_linear = (*linear_potentiometer).read_position() - delta_linear;
        }

        elapsed_time = micros() - elapsed_time;
        delay = (delay_off - elapsed_time > 0) ? (delay_off - elapsed_time) : (1);
        portENABLE_INTERRUPTS();

        delayMicroseconds(delay);

        // TODO: include position correction

        _update_position();

        step_cnt++;

        #if DEBUG_FEEDBACK_STEPPER
            expected_delay += get_expected_step_time();
            tot_angle += delta_angle;
            tot_linear += delta_linear;
        #endif
    }

    #if DEBUG_FEEDBACK_STEPPER
        debug_t = micros() - debug_t;
        if (step_cnt == 0) return;
        Serial.print("Expected (avg) delay: "); Serial.print(expected_delay / step_cnt);
        Serial.print("\tMeasured (avg) delay: "); Serial.print(debug_t / step_cnt);
        Serial.print("\tEncoder reading: "); Serial.print(tot_angle);
        Serial.print("\tPotentiometer reading: "); Serial.println(tot_linear);
    #endif
}