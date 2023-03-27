#include <Arduino.h>

#define FULL_STEP_MODE 1
#define HALF_STEP_MODE 2
#define QUARTER_STEP_MODE 4
#define EIGHT_STEP_MODE 8
#define SIXTEENTH_STEP_MODE 16


#define ENABLE_PIN 18           // negated pin
#define SLEEP_PIN 21            // negated pin
#define RESET_PIN 19            // negated pin

#define STEP_PIN 12
#define DIRECTION_PIN 14

#define MS1_PIN 25
#define MS2_PIN 33
#define MS3_PIN 32

#define LIMIT_SWITCH_PIN 23


const int steps_per_turn = 200;
const float deg_per_full_step = 1.8;
const int delay_on = 1;     // microseconds

int enable = 1;
int reset = 0;
int sleep_ = 0;
int on = 0;

int delay_off;
int direction = HIGH;
int microstepping = FULL_STEP_MODE;
float rpm = 10;

#define RPM_TO_DELAY_OFF(rpm) ((int) ((((float) deg_per_full_step / (float) microstepping) * 60.0e6) / (360.0 * rpm) - (float) delay_on))

int limit_reached = 0;

void step();
void set_speed (float speed);
void change_direction (int dir = -1);
void set_microstepping (int mode);

void IRAM_ATTR limit_switch_isr();


void setup() {
    Serial.begin(9600);

    pinMode(ENABLE_PIN, OUTPUT);
    pinMode(RESET_PIN, OUTPUT);
    pinMode(SLEEP_PIN, OUTPUT);
    
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIRECTION_PIN, OUTPUT);

    pinMode(MS1_PIN, OUTPUT);
    pinMode(MS2_PIN, OUTPUT);
    pinMode(MS3_PIN, OUTPUT);

    pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);
    attachInterrupt(LIMIT_SWITCH_PIN, limit_switch_isr, FALLING);


    digitalWrite(ENABLE_PIN, !enable);
    digitalWrite(RESET_PIN, !reset);
    digitalWrite(SLEEP_PIN, !sleep_);
    
    digitalWrite(DIRECTION_PIN, direction);

    set_microstepping(microstepping);

    set_speed(rpm);
}


void loop() {
    int i;
    
    if (on) {
        step();
    }

    if (Serial.available()) {
        char c;
        c = Serial.read();
        //delay(1000);
        switch (c) {
            case 'o': on=1-on; break;
            case 's': sleep_=1-sleep_;
                digitalWrite(SLEEP_PIN, !sleep_); break;
            case 'c': change_direction(); break;
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
            default: break;
        }
        char str[100];
        sprintf(str, "RPM: %.2f\tDirection: %d\tMicrostepping: %d\tSleep: %d\tOn: %d\tDelay off: %d",
                rpm, direction, microstepping, sleep_, on, delay_off);
        Serial.println(str);
    }

    if (limit_reached) {
        float prev_rpm = rpm;
        int prev_mode = microstepping;

        Serial.println("Limit reached");

        change_direction();
        set_microstepping(FULL_STEP_MODE);
        set_speed(400);


        while (!digitalRead(LIMIT_SWITCH_PIN)) {
            step();
        }

        set_microstepping(prev_mode);
        set_speed(prev_rpm);

        attachInterrupt(LIMIT_SWITCH_PIN, limit_switch_isr, FALLING);
        limit_reached--;
    }
}


void step() {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(delay_on);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(delay_off);
}


void set_speed (float speed) {
    int new_delay;

    rpm = speed;
    new_delay = RPM_TO_DELAY_OFF(rpm);
    new_delay = (new_delay < 1000000) ? (new_delay):(1000000);
    delay_off = new_delay;
}


void change_direction (int dir) {
    if (dir == direction)
        return;
    
    if (dir == -1) {
        direction = 1 - direction;
    } else {
        direction = dir;
    }

    digitalWrite(DIRECTION_PIN, direction);
    delayMicroseconds(10000);
}


void set_microstepping (int mode) {
    int ms1, ms2, ms3;

    if (microstepping == mode)
        return;

    switch (mode) {
        case FULL_STEP_MODE:
            ms1 = 0; ms2 = 0; ms3 = 0; break;
        case HALF_STEP_MODE:
            ms1 = 1; ms2 = 0; ms3 = 0; break;
        case QUARTER_STEP_MODE:
            ms1 = 0; ms2 = 1; ms3 = 0; break;
        case EIGHT_STEP_MODE:
            ms1 = 1; ms2 = 1; ms3 = 0; break;
        case SIXTEENTH_STEP_MODE:
            ms1 = 1; ms2 = 1; ms3 = 1; break;
        default: break;
    }

    digitalWrite(MS1_PIN, ms1);
    digitalWrite(MS2_PIN, ms2);
    digitalWrite(MS3_PIN, ms3);
    
    microstepping = mode;
    set_speed(rpm);
}


void IRAM_ATTR limit_switch_isr() {
    int t_begin, status;
    
    detachInterrupt(LIMIT_SWITCH_PIN);

    t_begin = millis();
    while (millis() - t_begin < 50);

    status = digitalRead(LIMIT_SWITCH_PIN);

    if (status) {
        attachInterrupt(LIMIT_SWITCH_PIN, limit_switch_isr, FALLING);
        return;
    }

    on = 0;

    limit_reached++; 
}