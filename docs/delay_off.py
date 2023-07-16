import numpy as np
import matplotlib.pyplot as plt


SIXTEENTH_MAX_RPM = 120
EIGHT_MODE_MAX_RPM = 180
QUARTER_MODE_MAX_RPM = 240
HALF_MODE_MAX_RPM = 300
MIN_RPM = 100
MAX_RPM = 600

AVERAGE_I2C_ENCODER_READ_TIME = 230
AVERAGE_ADC_ENCODER_READ_TIME = 80


def main():
    rpm = np.arange(MIN_RPM, MAX_RPM, 0.01)
    delay_off = [delay_off_f(x) for x in rpm]
    avg_i2c_encoder_read_time = [AVERAGE_I2C_ENCODER_READ_TIME for x in rpm]
    avg_adc_encoder_read_time = [AVERAGE_ADC_ENCODER_READ_TIME for x in rpm]

    plt.figure(1)
    plt.title("Delay off per RPM")
    plt.xlabel("Speed [RPM]")
    plt.ylabel("Delay off [microsec]")
    plt.grid('both', 'both')
    plt.ylim(0, 1100)

    plt.plot(rpm, delay_off, label="Delay off f(rpm, microstepping)")
    plt.plot(rpm, avg_i2c_encoder_read_time, ':', label="Average I2C encoder read time")
    plt.plot(rpm, avg_adc_encoder_read_time, ':', label="Average ADC encoder read time")

    plt.legend()
    plt.show()


def _delay_off_f(ms, rpm):
    deg_per_full_step = 1.8
    delay_on = 1
    return ((deg_per_full_step / ms) * 60.0e6) / (360.0 * rpm) - delay_on


def delay_off_f(rpm):
    ms = 0
    if rpm <= SIXTEENTH_MAX_RPM:
        ms = 16
    elif rpm <= EIGHT_MODE_MAX_RPM:
        ms = 8
    elif rpm <= QUARTER_MODE_MAX_RPM:
        ms = 4
    elif rpm <= HALF_MODE_MAX_RPM:
        ms = 2
    else:
        ms = 1
    
    return _delay_off_f(ms, rpm)
        
    


if __name__=='__main__':
    main()