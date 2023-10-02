# Camilla - CAMbio Intrinsicamente LAborioso

Software for the Phoenix translating gearbox, using a stepper motor.


## Cores

The main funcitonalities are split between the two ESP32's cores, contained in two files in the `src/` folder
- `core1`: manages the stepper motor and the buttons
- `core0`: manages CAN and the webserver


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