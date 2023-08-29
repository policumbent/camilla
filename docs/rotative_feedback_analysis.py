import numpy as np
import matplotlib.pyplot as plt
from scipy.fft import fft


FILENAME = "./docs/rotative_feedback_data_32_espi2c_case.txt"
REMOVE_SPIKES = False


def main():
    delta_pos = []
    delta_angle = []
    error = []

    with open(FILENAME) as file:
        for line in file:
            if line.strip() == "" or line[0] not in "0123456789-":
                continue
            elem = line.strip().split("\t")
            pos = abs(int(elem[0]))
            ang = abs(int(elem[1]) / 4095 * 3200)
            err = int(elem[2])

            if REMOVE_SPIKES and ang > 100:
                continue

            delta_pos.append(pos)
            delta_angle.append(ang)
            error.append(err)


    print("DELTA ANGLE STATISTICS")
    print("Mean: ", np.mean(delta_angle))
    print("Standard deviation: ", np.std(delta_angle))
    print("Variance: ", np.var(delta_angle))
    print("Maximum: ", np.max(delta_angle))

    print("ERROR STATISTICS")
    print("Mean: ", np.mean(error))
    print("Standard deviation: ", np.std(error))
    print("Variance: ", np.var(error))
    print("Maximum: ", np.max(error))


    index = [i for i in range(len(delta_pos))]

    plt.figure(1)
    plt.title("Rotative feedback analysis")
    plt.grid('both', 'both')

    plt.plot(index, delta_pos, label="Delta position expected")
    plt.plot(index, delta_angle, label="Delta angle measured")
    plt.plot(index, error, label="Error")

    
    x = np.array(delta_angle)
    y = fft(x)
    
    plt.figure(2)
    plt.title("Delta angle fft")
    plt.grid('both', 'both')

    plt.plot(abs(y))


    plt.legend()
    plt.show()

    


if __name__ == '__main__':
    main()