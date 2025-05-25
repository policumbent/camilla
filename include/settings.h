#pragma once

#define CERBERUS_NUM_GEARS  10
#define PHOENIX_NUM_GEARS   12

#define NUM_GEARS CERBERUS_NUM_GEARS


#define DEFAULT_MODE_DISABLED      0    // first mode encountered: menu
#define DEFAULT_MODE_GEARS         1    // first mode encountered: gears
#define DEFAULT_MODE_WHILE_PRESSED 2    // first mode encountered: while_pressed

#define DEFAULT_MODE               DEFAULT_MODE_WHILE_PRESSED

/* if 1, uses manual overshoot version of gears;
 * if 0, uses no overshoot version of gears */
#define NEVADA_MODE 0

#define GEARS_SETUP 0

#define MICROSTEPPING_ENABLED        1
#define ENCODER_CONNECTED            0
#define POTENTIOMETER_CONNECTED      0
#define LIMIT_SWITCH_BEGIN_CONNECTED 0
#define LIMIT_SWITCH_END_CONNECTED   0

#define LIMIT_SWITCH_AS_REFERENCE 1
#if LIMIT_SWITCH_AS_REFERENCE
    #define ZERO_POSITION_AT_BEGIN 0
#endif

#define SERIAL_CALIBRATION 0


#define DEBUG 1

#if DEBUG
    #define DEBUG_CORES         1
    #define DEBUG_MOTOR         0
    #define DEBUG_MEMORY        1
    #define DEBUG_BUTTONS       1
    #define DEBUG_LIMIT_SWITCH  1
    #define DEBUG_GEARS         1
    
    #if ENCODER_CONNECTED
        #define DEBUG_ENCODER       2
    #endif
    
    #if POTENTIOMETER_CONNECTED
        #define DEBUG_POTENTIOMETER 1
    #endif

    #define DEBUG_CAN 2

    #define DEBUG_CALIBRATION   1
#endif