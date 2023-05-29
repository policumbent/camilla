import numpy as np
import matplotlib.pyplot as plt


SIXTEENTH_MAX_RPM = 120
EIGHT_MODE_MAX_RPM = 180
QUARTER_MODE_MAX_RPM = 240
HALF_MODE_MAX_RPM = 300
MIN_MOVE_RPM = 100
MAX_RPM = 600

MAX_ACCELERATION_STEPS = 800

AVERAGE_I2C_ENCODER_READ_TIME = 300
AVERAGE_ADC_ENCODER_READ_TIME = 80


def main():
    # All expressed in sixteenth steps
    gears = [0, 12800, 25600, 38400, 51200, 64000, 76800, 89600, 102400, 115200, 128000, 140800]
    positions = np.arange(min(gears), max(gears), 1)
    
    speeds = []
    delay_offs = []

    for i in range(len(gears) - 1):
        start_pos = gears[i]
        target_pos = gears[i+1]

        for pos in range(start_pos, target_pos):
            rpm = speed_f(pos, start_pos, target_pos)
            d_off = delay_off_f(rpm)

            speeds.append(rpm)
            delay_offs.append(d_off)

    avg_i2c_encoder_read_time = [AVERAGE_I2C_ENCODER_READ_TIME for x in positions]
    avg_adc_encoder_read_time = [AVERAGE_ADC_ENCODER_READ_TIME for x in positions]

    plt.figure(1)
    plt.title("Speed per position")
    plt.xlabel("Position [1/16 step]")
    plt.ylabel("Speed [RPM] / Delay off [microsec]")
    plt.grid('both', 'both')
    plt.ylim(0, 1100)

    plt.plot(positions, speeds, label="Speed per position")
    plt.plot(positions, delay_offs, label="Delay off per position")
    plt.plot(positions, avg_i2c_encoder_read_time, ':', label="Average I2C encoder read time")
    plt.plot(positions, avg_adc_encoder_read_time, ':', label="Average ADC encoder read time")

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


def speed_f(pos, start_pos, target_pos):
    speed = 0
    accel_steps = MAX_ACCELERATION_STEPS
    
    if abs(target_pos - start_pos) < 2 * MAX_ACCELERATION_STEPS:
        accel_steps = abs(target_pos - start_pos) / 2


    if pos < start_pos + accel_steps:
        speed = MIN_MOVE_RPM + ((float) (pos - start_pos) / (float) (MAX_ACCELERATION_STEPS)) * (MAX_RPM - MIN_MOVE_RPM)    
    elif pos > target_pos - accel_steps:
        speed = MIN_MOVE_RPM + ((float) (target_pos - pos) / (float) (MAX_ACCELERATION_STEPS)) * (MAX_RPM - MIN_MOVE_RPM)
    else:
        speed = MAX_RPM
    
    return speed
    


if __name__=='__main__':
    main()