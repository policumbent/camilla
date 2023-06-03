import numpy as np
import matplotlib.pyplot as plt


SIXTEENTH_MAX_RPM = 120
EIGHT_MODE_MAX_RPM = 180
QUARTER_MODE_MAX_RPM = 240
HALF_MODE_MAX_RPM = 300
MIN_RPM = 100
MAX_RPM = 600

AVERAGE_I2C_ENCODER_READ_TIME = 300
AVERAGE_ADC_ENCODER_READ_TIME = 80


def main():
    i2c_encoder_readings = []
    adc_encoder_readings = []

    with open("enc_read_log1.txt") as file:
        for line in file:
            readings = line.split(";")
            i2c = int(readings[0])
            adc = int(readings[1])
            i2c_encoder_readings.append(i2c)
            adc_encoder_readings.append(adc)

    positions = [i for i in range(len(i2c_encoder_readings))]

    print(f"Number of readings: {len(i2c_encoder_readings)}")

    plt.figure(1)
    plt.title("Encoder for positions (full steps)")
    plt.xlabel("Position [1/16 step]")
    plt.ylabel("Encoder reading [0-4095]")
    plt.grid('both', 'both')

    plt.plot(positions, i2c_encoder_readings, label="Encoder I2C position f(position)")
    plt.plot(positions, adc_encoder_readings, label="Encoder ADC position f(position)")

    plt.legend()
    plt.show()
        
    


if __name__=='__main__':
    main()