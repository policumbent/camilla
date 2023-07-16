#pragma once

#define DEBUG 1

#if DEBUG
    #define DEBUG_CORES         1
    #define DEBUG_MOTOR         0
    #define DEBUG_MEMORY        0
    #define DEBUG_BUTTONS       1
    #define DEBUG_LIMIT_SWITCH  1
    #define DEBUG_GEARS         1
    #define DEBUG_ENCODER       2
    #define DEBUG_POTENTIOMETER 1

    #define DEBUG_CAN 1
#endif