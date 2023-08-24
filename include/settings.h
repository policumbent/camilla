#pragma once

#define SERIAL_CALIBRATION  0

#define ENCODER_CONNECTED       0
#define POTENTIOMETER_CONNECTED 0

#define DEBUG 1

#if DEBUG
    #define DEBUG_CORES         1
    #define DEBUG_MOTOR         0
    #define DEBUG_MEMORY        0
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