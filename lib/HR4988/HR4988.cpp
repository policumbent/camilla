#include "HR4988.h"

HR4988 :: HR4988 (uint8_t step_pin, uint8_t direction_pin,
                  uint8_t ms1_pin, uint8_t ms2_pin, uint8_t ms3_pin,
                  uint8_t enable_pin, uint8_t sleep_pin, uint8_t reset_pin,
                  int full_steps_per_turn, float deg_per_full_step,
                  int8_t cw_direction_sign ) {
    this->step_pin = step_pin;
    this->direction_pin = direction_pin;
    this->ms1_pin = ms1_pin;
    this->ms2_pin = ms2_pin;
    this->ms3_pin = ms3_pin;
    this->enable_pin = enable_pin;
    this->sleep_pin = sleep_pin;
    this->reset_pin = reset_pin;
    this->full_steps_per_turn = full_steps_per_turn;
    this->deg_per_full_step = deg_per_full_step;
    this->cw_direction_sign = cw_direction_sign;

    setup();
}


HR4988 :: HR4988 (uint8_t step_pin, uint8_t direction_pin,
                  uint8_t ms1_pin, uint8_t ms2_pin, uint8_t ms3_pin,
                  uint8_t enable_pin,
                  int full_steps_per_turn, float deg_per_full_step,
                  int8_t cw_direction_sign ) {
    this->step_pin = step_pin;
    this->direction_pin = direction_pin;
    this->ms1_pin = ms1_pin;
    this->ms2_pin = ms2_pin;
    this->ms3_pin = ms3_pin;
    this->enable_pin = enable_pin;
    this->sleep_pin = 0;
    this->reset_pin = 0;
    this->full_steps_per_turn = full_steps_per_turn;
    this->deg_per_full_step = deg_per_full_step;
    this->cw_direction_sign = cw_direction_sign;

    setup();
}


HR4988 :: HR4988 (uint8_t step_pin, uint8_t direction_pin,
                  uint8_t enable_pin,
                  int full_steps_per_turn, float deg_per_full_step,
                  int8_t cw_direction_sign) {
    this->step_pin = step_pin;
    this->direction_pin = direction_pin;
    this->ms1_pin = 0;
    this->ms2_pin = 0;
    this->ms3_pin = 0;
    this->enable_pin = enable_pin;
    this->sleep_pin = 0;
    this->reset_pin = 0;
    this->full_steps_per_turn = full_steps_per_turn;
    this->deg_per_full_step = deg_per_full_step;
    this->cw_direction_sign = cw_direction_sign;

    setup();
}


void HR4988 :: setup() {
    pinMode(step_pin, OUTPUT);
    pinMode(direction_pin, OUTPUT);

    microstepping_on = 0;
    if (ms1_pin != 0 && ms2_pin != 0 && ms3_pin != 0) {
        microstepping_on = 1;
        pinMode(ms1_pin, OUTPUT);
        pinMode(ms2_pin, OUTPUT);
        pinMode(ms3_pin, OUTPUT);
    }
    
    pinMode(enable_pin, OUTPUT);

    enable = 1;
    digitalWrite(enable_pin, !enable);

    reset = -1;
    if (reset_pin != 0) {
        reset = 0;
        pinMode(reset_pin, OUTPUT);
        digitalWrite(reset_pin, !reset);
    }
    _sleep = -1;
    if (sleep_pin != 0) {
        _sleep = 0;
        pinMode(sleep_pin, OUTPUT);
        digitalWrite(sleep_pin, !_sleep);
    }

    position_sixteenth = 0;
    direction = 0;
    microstepping = FULL_STEP_MODE;
    position_change = POSITION_CHANGE_FULL_MODE;
    rpm = 60.0;
    set_direction(direction);
    set_microstepping(microstepping);
    set_speed(rpm);
}


void HR4988 :: move_const_speed(int target_pos, float rpm) {
    int start_pos = position_sixteenth;
    uint8_t dir;

    set_speed(rpm);

    if (start_pos < target_pos) {
        dir = (cw_direction_sign == 1) ? CW : CCW;
    }
    else {
        dir = (cw_direction_sign == 1) ? CCW : CW;
    }

    if (direction != dir) {
        set_direction(dir);
    }
    if (rpm != this->rpm) {
        set_speed(rpm);
    }

    while (position_sixteenth != target_pos) {
        step();
    }
}


void HR4988 :: move(int target_pos) {
    int start_pos = position_sixteenth;
    long int elapsed_time, delay;
    int step_cnt;
    
    #if DEBUG_HR4988
        Serial.print("[HR4988] Shift from "); Serial.print(start_pos); Serial.print(" to "); Serial.println(target_pos);
        long int debug_t = micros();
        int expected_delay = 0;
    #endif
    
    step_cnt = 0;

    while (position_sixteenth != target_pos) {

        portDISABLE_INTERRUPTS();
        elapsed_time = micros();

        _move_set_speed_direction(start_pos, target_pos);

        _step_no_delay_off();

        elapsed_time = micros() - elapsed_time;
        delay = (delay_off - elapsed_time > 0) ? (delay_off - elapsed_time) : (1);
        portENABLE_INTERRUPTS();

        delayMicroseconds(delay);

        _update_position();

        step_cnt++;

        #if DEBUG_HR4988
            expected_delay += get_expected_step_time();
        #endif
    }

    #if DEBUG_HR4988
        debug_t = micros() - debug_t;
        if (step_cnt == 0) return;
        Serial.print("Expected (avg) delay: "); Serial.print(expected_delay / step_cnt);
        Serial.print("\tMeasured (avg) delay: "); Serial.println(debug_t / step_cnt);
    #endif
}


void HR4988 :: _move_set_speed_direction(int start_pos, int target_pos) {
    uint8_t dir;
    float speed;
    int accel_steps;

    /*
        Trapezoidal speed behavior
        - first part accelerating
        - middle part at constant max speed
        - end part decelerating
        If there is not enough steps to perform MAX_ACCELERATION_STEPS both in acceleration and deceleration,
            then less steps are taken into account, however speed scaling still refers to max value in
            order to mantain acceleration value constant and not accelerate faster if there are less steps
            to be performed
    */

    if (abs(target_pos - start_pos) < 2 * MAX_ACCELERATION_STEPS) {
        accel_steps = abs(target_pos - start_pos) / 2;
    } else {
        accel_steps = MAX_ACCELERATION_STEPS;
    }

    if (start_pos < target_pos) {
        dir = (cw_direction_sign == 1) ? CW : CCW;

        if (position_sixteenth < start_pos + accel_steps) {
            speed = MIN_MOVE_RPM + ((float) (position_sixteenth - start_pos) / (float) (MAX_ACCELERATION_STEPS)) * (MAX_RPM - MIN_MOVE_RPM);
        }
        else if (position_sixteenth > target_pos - accel_steps) {
            speed = MIN_MOVE_RPM + ((float) (target_pos - position_sixteenth) / (float) (MAX_ACCELERATION_STEPS)) * (MAX_RPM - MIN_MOVE_RPM);
        }
        else {
            speed = MAX_RPM;
        }
    }
    else {
        dir = (cw_direction_sign == 1) ? CCW : CW;

        if (position_sixteenth > start_pos - accel_steps) {
            speed = MIN_MOVE_RPM + ((float) (start_pos - position_sixteenth) / (float) (MAX_ACCELERATION_STEPS)) * (MAX_RPM - MIN_MOVE_RPM);
        }
        else if (position_sixteenth < target_pos + accel_steps) {
            speed = MIN_MOVE_RPM + ((float) (position_sixteenth - target_pos) / (float) (MAX_ACCELERATION_STEPS)) * (MAX_RPM - MIN_MOVE_RPM);
        }
        else {
            speed = MAX_RPM;
        }
    }

    if (direction != dir) {
        set_direction(dir);
    }
    if (rpm != speed) {
        set_speed(speed);
    }
}


void HR4988 :: _update_position() {
    if (direction == CW) {
        position_sixteenth += cw_direction_sign * position_change;
    } else {
        position_sixteenth -= cw_direction_sign * position_change;
    }
}


void HR4988 :: _step_no_delay_off() {
    digitalWrite(step_pin, HIGH);
    delayMicroseconds(delay_on);
    digitalWrite(step_pin, LOW);
}


void HR4988 :: step() {
    _step_no_delay_off();
    delayMicroseconds(delay_off);

    _update_position();
}


void HR4988 :: set_position(int position) {
    this->position_sixteenth = position;
}


int HR4988 :: get_position() {
    return position_sixteenth;
}


void HR4988 :: set_speed(float speed) {
    uint8_t ms;

    rpm = speed;

    if (rpm <= SIXTEENTH_MAX_RPM) {
        ms = SIXTEENTH_STEP_MODE;
    } else if (rpm <= EIGHT_MODE_MAX_RPM) {
        ms = EIGHT_STEP_MODE;
    } else if (rpm <= QUARTER_MODE_MAX_RPM) {
        ms = QUARTER_STEP_MODE;     // check vibrations
    } else if (rpm <= HALF_MODE_MAX_RPM) {
        ms = HALF_STEP_MODE;        // check vibrations
    } else {
        ms = FULL_STEP_MODE;
    }

    if (microstepping != ms) {
        set_microstepping(ms);
    }

    delay_off = (int) ((((float) deg_per_full_step / (float) microstepping) * 60.0e6) / (360.0 * rpm) - (float) delay_on);
    //delay_off = ((float) 1000000 / ((float) full_steps_per_turn * (float) microstepping * 60.0 * rpm));
    delay_off = (delay_off < 1000000) ? delay_off : 1000000;
}


float HR4988 :: get_speed() {
    return rpm;
}


void HR4988 :: change_direction() {
    direction = 1 - direction;
    digitalWrite(direction_pin, direction);
    delayMicroseconds(DELAY_CHANGE_DIRECTION);
}


void HR4988 :: set_direction(uint8_t dir) {
    if (direction == dir)
        return;
    
    change_direction();
}


uint8_t HR4988 :: get_direction() {
    return direction;
}


void HR4988 :: set_microstepping(uint8_t mode) {
    uint8_t ms1, ms2, ms3;

    if (microstepping == mode)
        return;

    if (!microstepping_on) {
        microstepping = FULL_STEP_MODE;
        position_change = POSITION_CHANGE_FULL_MODE;
        return;
    }

    microstepping = mode;

    switch (mode) {
        case FULL_STEP_MODE:
            ms1 = 0; ms2 = 0; ms3 = 0;
            position_change = POSITION_CHANGE_FULL_MODE; break;
        case HALF_STEP_MODE:
            ms1 = 1; ms2 = 0; ms3 = 0;
            position_change = POSITION_CHANGE_HALF_MODE; break;
        case QUARTER_STEP_MODE:
            ms1 = 0; ms2 = 1; ms3 = 0;
            position_change = POSITION_CHANGE_QUARTER_MODE; break;
        case EIGHT_STEP_MODE:
            ms1 = 1; ms2 = 1; ms3 = 0;
            position_change = POSITION_CHANGE_EIGHT_MODE; break;
        case SIXTEENTH_STEP_MODE:
            ms1 = 1; ms2 = 1; ms3 = 1;
            position_change = POSITION_CHANGE_SIXTEENTH_MODE; break;
        default: break;
    }

    digitalWrite(ms1_pin, ms1);
    digitalWrite(ms2_pin, ms2);
    digitalWrite(ms3_pin, ms3);
    
    set_speed(rpm);
}


uint8_t HR4988 :: get_microstepping() {
    return microstepping;
}


int HR4988 :: get_delta_position_360_degrees_rotation() {
    return full_steps_per_turn * SIXTEENTH_STEP_MODE;
}


int HR4988 :: get_expected_step_time() {
    return delay_on + delay_off;
}


int8_t HR4988 :: get_cw_direction_sign() {
    return cw_direction_sign;
}


void HR4988 :: debug_serial_control() {
    uint8_t end = 0;
    int steps_per_activation = full_steps_per_turn / 4;

    Serial.println("Debug from serial initialized (read switch case from HR4988.cpp)");

    while (!end) {
        
        for (int i=0; i<steps_per_activation; i++) {
            step();
        }

        if (Serial.available()) {
            char c;
            c = Serial.read();

            switch (c) {
                case 'c': change_direction(); break;
                case '^': steps_per_activation += 1; break;
                case 'i': steps_per_activation += 10; break;
                case 'I': steps_per_activation += 100; break;
                case 'v': steps_per_activation -= 1; break;
                case 'd': steps_per_activation -= 10; break;
                case 'D': steps_per_activation -= 100; break;
                case '*': rpm += 100; set_speed(rpm); break;
                case '/': rpm -= 100; set_speed(rpm); break;
                case '+': rpm += 10; set_speed(rpm); break;
                case '-': rpm -= 10; set_speed(rpm); break;
                case ',': rpm += 1; set_speed(rpm); break;
                case '.': rpm -= 1; set_speed(rpm); break;
                case '1': set_microstepping(FULL_STEP_MODE); break;
                case '2': set_microstepping(HALF_STEP_MODE); break;
                case '4': set_microstepping(QUARTER_STEP_MODE); break;
                case '8': set_microstepping(EIGHT_STEP_MODE); break;
                case '6': set_microstepping(SIXTEENTH_STEP_MODE); break;
                case 'e': end = 1; break;
                default: break;
            }
            print_status();
        }
    }
}


void HR4988 :: print_status() {
    if (!Serial)
        return;
        
    char str[100];
    sprintf(str, "Position: %d\tRPM: %.2f\tDirection: %d\tMicrostepping: %d\tSleep: %d\tDelay off: %d",
            position_sixteenth, rpm, direction, microstepping, _sleep, delay_off);
    Serial.println(str);
}