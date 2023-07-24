#pragma once

#include "main.h"

#include <esp32_can.h>
#include "policanbent.h"


#define CAN_TX_PIN GPIO_NUM_23
#define CAN_RX_PIN GPIO_NUM_19


void webserver_calibration();