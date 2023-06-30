import matplotlib.pyplot as plt
import csv

flags = ['-O1', '-O2', '-O3', '-Os', '-Ofast', '-Og']
for flag in flags:
    x = []
    y = []
    with open('k.csv', 'r') as csvfile:
        plots = csv.reader(csvfile, delimiter=',')
        headers = next(plots)

        for row in plots:
            if(row[2] != flag):
                continue
            x.append(row[0])
            y.append(row[1])

    plt.bar(x, y, color='g', width=0.92, label="Time")
    plt.xticks(x[::1], x[::1],)
    plt.yticks(y[::1], y[::1], fontsize=6)
    plt.xlabel('n')
    plt.ylabel('times')
    plt.title('Time program for n with optimization ' + flag)
    plt.legend()
    plt.savefig(flag + '.png')