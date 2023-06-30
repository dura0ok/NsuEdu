import matplotlib.pyplot as plt
import csv

x = []
y = []

with open('stat.csv', 'r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    headers = next(plots)

    for row in plots:
        x.append(row[0])
        y.append(row[1])

plt.bar(x, y, color='g', width=0.72, label="Time")
plt.xticks(x[::10], x[::10])
plt.yticks(y[::10], y[::10])
plt.xlabel('n')
plt.ylabel('times')
plt.title('Time program for n')
plt.legend()
plt.show()
