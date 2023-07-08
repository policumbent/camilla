import numpy as np
import matplotlib.pyplot as plt


def main():
    delta_pos = []
    delta_angle = []
    error = []

    with open("rotative_feedback_data.txt") as file:
        for line in file:
            if line.strip() == "":
                continue
            elem = line.strip().split("\t")
            pos = int(elem[0])
            ang = int(elem[1]) / 4095 * 3200
            delta_pos.append(pos)
            delta_angle.append(ang)
            error.append(pos - ang)

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