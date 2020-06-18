import numpy as np


def average_random_edges():
    rnd_edges = np.zeros(300)


    for i in range(10):
        with open("out_random_edges_%d_log.txt" %i, "r") as file_one:
            file_one.readline()
            for j in range(300):
                rnd_edges[j] += float(file_one.readline().split()[1])

    rnd_edges = rnd_edges / 10

    with open("random_edges.txt", "w") as file_one:
        file_one.write("edge\tRed Ratio\n")
        for i in range(300):
            file_one.write("%d\t%f\n" %(i, rnd_edges[i]))

def average_random_source():
    rnd_edges = np.zeros(300)


    for i in range(10):
        with open("out_random_edges_%d_log.txt" %i, "r") as file_one:
            file_one.readline()
            for j in range(300):
                rnd_edges[j] += float(file_one.readline().split()[1])

    rnd_edges = rnd_edges / 10

    with open("random_source.txt", "w") as file_one:
        file_one.write("edge\tRed Ratio\n")
        for i in range(300):
            file_one.write("%d\t%f\n" %(i, rnd_edges[i]))



