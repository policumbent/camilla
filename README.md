# Camilla - CAmbio Intrinsicamente LAborioso

## HR4988 stepper motor 

Testing software for the Phoenix gearbox, using a stepper motor.

## Main functionalities

The main funcitonalities are split between the two ESP32's cores. They are
contained in two files in the ``src/`` folder.

Global variables and functions are declared in ``include/main.h``

## Libraries

There are several libraries used by this software:
- Button: manages the gearbox command button and the limit switches
- HR4988: manages the functionalities of the stepper motor
