import numpy as np
import matplotlib.pyplot as plt


FILENAME = "rotative_feedback_data_32_espi2c.txt"
REMOVE_SPIKES = True


def main():
    delta_pos = []
    delta_angle = []
    error = []

    with open(FILENAME) as file:
        for line in file:
            if line.strip() == "":
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

    plt.legend()
    plt.show()

    


if __name__ == '__main__':
    main()