#include "AS5600.h"


AS5600 :: AS5600() {
    analog_pin = 0;

    setup();
}


AS5600 :: AS5600 (uint8_t analog_pin) {
    this->analog_pin = analog_pin;

    setup();
}


/*
void AS5600 :: setup() {
    Wire.begin();
    Wire.setClock(8000000);

    if (analog_pin != 0) {
        pinMode(analog_pin, INPUT);
    }
}
*/


void AS5600 :: setup() {
    i2c_master_port = 0;
    i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE
    };
    i2c_conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &i2c_conf);

    i2c_driver_install(i2c_master_port, I2C_MODE_MASTER, 0, 0, 0);      // other parameters set to 0 since ignored in master mode

    if (analog_pin != 0) {
        pinMode(analog_pin, INPUT);
    }
}


uint16_t AS5600 :: read_angle_output() {
    angle = analogRead(analog_pin);
    //angle = adc1_get_raw(ADC1_CHANNEL_6);
    return angle;
}


int AS5600 :: get_angle() {
    return angle;
}


void AS5600 :: calibration(HR4988 &stepper_motor) {
    char c;

    Serial.println("\nEncoder calibration");

    while (Serial.available()) c = Serial.read();   // clear the read buffer

    Serial.println("Connect the PGO pin to GND, then press any key");
    while (!Serial.available()) delay(10); c = Serial.read();
    
    pinMode(analog_pin, OUTPUT);
    digitalWrite(analog_pin, LOW);
    delay(200);

    Serial.println("Disconnect the OUT pin, then press any key");
    while (!Serial.available()) delay(10); c = Serial.read();

    int start_pos = stepper_motor.get_position();
    int target_pos;
    if (stepper_motor.get_cw_direction_sign() == 1) {
        target_pos = start_pos + stepper_motor.get_delta_position_360_degrees_rotation() - 100;
    } else {
        target_pos = start_pos - stepper_motor.get_delta_position_360_degrees_rotation() + 100;
    }

    stepper_motor.move_const_speed(target_pos, 10.0);

    Serial.println("Resconnect the OUT pin, then press any key");
    while (!Serial.available()) delay(10); c = Serial.read();

    digitalWrite(analog_pin, LOW);
    delay(200);
    pinMode(analog_pin, INPUT);

    Serial.println("Disconnect the PGO pin from GND, then press any key");
    while (!Serial.available()) delay(10); c = Serial.read();

    Serial.print("Encoder calibration finished\n");
}


/*
int AS5600 :: read_angle() {
    int lowbyte, highbyte;

    //7:0 - bits
    Wire.beginTransmission(0x36); //connect to the sensor
    Wire.write(0x0D); //figure 21 - register map: Raw angle (7:0)
    Wire.endTransmission(); //end transmission
    Wire.requestFrom(0x36, 1); //request from the sensor

    while (Wire.available() == 0); //wait until it becomes available
    lowbyte = Wire.read(); //Reading the data after the request

    //11:8 - 4 bits
    Wire.beginTransmission(0x36);
    Wire.write(0x0C); //figure 21 - register map: Raw angle (11:8)
    Wire.endTransmission();
    Wire.requestFrom(0x36, 1);

    while (Wire.available() == 0);
    highbyte = Wire.read();

    //4 bits have to be shifted to its proper place as we want to build a 12-bit number
    highbyte = highbyte << 8; //shifting to left
    //What is happening here is the following: The variable is being shifted by 8 bits to the left:
    //Initial value: 00000000|00001111 (word = 16 bits or 2 bytes)
    //Left shifting by eight bits: 00001111|00000000 so, the high byte is filled in

    //Finally, we combine (bitwise OR) the two numbers:
    //High: 00001111|00000000
    //Low:  00000000|00001111
    //      -----------------
    //H|L:  00001111|00001111
    angle = highbyte | lowbyte; //int is 16 bits (as well as the word)

    //We need to calculate the angle:
    //12 bit -> 4096 different levels: 360° is divided into 4096 equal parts:
    //360/4096 = 0.087890625
    //Multiply the output of the encoder with 0.087890625
    //angle = angle * 0.087890625;

    //Serial.print("Deg angle: ");
    //Serial.println(degAngle, 2); //absolute position of the encoder within the 0-360 circle

    return angle;
}
*/


int AS5600 :: read_angle() {
    uint8_t write_byte;
    uint8_t low_byte, high_byte;

    write_byte = AS5600_I2C_LOW_BYTE_CMD;
    i2c_master_write_to_device(i2c_master_port, AS5600_I2C_ADDRESS, &write_byte, 1, 1000/portTICK_RATE_MS);
    i2c_master_read_from_device(i2c_master_port, AS5600_I2C_ADDRESS, &low_byte, 1, 1000/portTICK_RATE_MS);

    write_byte = AS5600_I2C_HIGH_BYTE_CMD;
    i2c_master_write_to_device(i2c_master_port, AS5600_I2C_ADDRESS, &write_byte, 1, 1000/portTICK_RATE_MS);
    i2c_master_read_from_device(i2c_master_port, AS5600_I2C_ADDRESS, &high_byte, 1, 1000/portTICK_RATE_MS);

    angle = (((int) high_byte) << 8) | (int) low_byte;

    return angle;
}