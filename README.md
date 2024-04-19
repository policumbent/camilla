# Camilla - CAMbio Intrinsicamente LAborioso

Software for the Phoenix translating gearbox, using a stepper motor


## Settings

The main settings and debug options can be configured by changing values of constants defined in `include/settings.h`


## Cores

The main functionalities are split between the two ESP32's cores, contained in two files in the `src/` folder
- `core1`: manages the stepper motor, the buttons and limit switches
- `core0`: manages CAN, the webserver and the interrupts


## Libraries

There are several libraries used by this software (capital letters libraries implement one class):
- `HR4988`: acts as a software driver for the stepper motor, providing basic function to move the motor interacting with the HR4988 driver
- `FeedbackStepper`: manages the stepper motor at a higher level of abstraction, including feedbacks and buttons management
- `button`: provides basic functions to setup a button or a limit switch as an interrupt
- `AS5600`: provides functions to read the magnetic rotative encoder using I2C protocol
- `Potentiometer`: provides functions to read the linear potetiometer through an analog pin
- `Webserver`: manages the creation and the interaction with a server hosted in the ESP to calibrate the gearshift (`js`, `css` and `html` sent to the client are inside the `data/` folder)
- `Memory`: manages reading and writing from Flash ESP, in particular gear positions
- `policanbent:` manages CAN communication

## TODO list

- Fork [esp32_can](https://github.com/collin80/esp32_can)
    - Add core selection for `void task_LowLevelRX` in
    `uint32_t ESP32CAN::init(uint32_t ul_baudrate)`