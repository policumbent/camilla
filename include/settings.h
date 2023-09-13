#pragma once


#define NEVADA_MODE 1

#define GEARS_SETUP 0

#define MICROSTEPPING_ENABLED        1
#define ENCODER_CONNECTED            0
#define POTENTIOMETER_CONNECTED      0
#define LIMIT_SWITCH_BEGIN_CONNECTED 1
#define LIMIT_SWITCH_END_CONNECTED   1
#define ZERO_POSITION_AT_BEGIN       0

#define SERIAL_CALIBRATION  1


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

    #define DEBUG_CAN 1

    #define DEBUG_CALIBRATION   1
#endif