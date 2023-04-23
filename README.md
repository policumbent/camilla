# Camilla - CAmbio Intrinsicamente LAborioso

Testing software for the Phoenix gearbox, using a stepper motor.

## Main functionalities

The main funcitonalities are split between the two ESP32's cores. They are
contained in two files in the ``src/`` folder.

Global variables and functions are declared in ``include/main.h``

## Libraries

There are several libraries used by this software:
- HR4988: manages the stepper motor functionalities
- AS5600: manages the magnetic rotative encoder
- Button: manages the gearbox command button and the limit switches
- Memory: manages reading and writing from Flash ESP
