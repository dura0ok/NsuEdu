import matplotlib.pyplot as plt
import csv
  
x = []
y = []

flags = ['-O1', '-O2', '-O3', '-Os', '-Ofast', '-Og']
flag = flags[0]
with open('k.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter = ',')
    headers = next(plots)
      
    for row in plots:
        if row[2] != flag:
            continue
        x.append(row[0])
        y.append(row[1])
  

x_ticks = [str(tick) for tick in range(1, len(x)+1, 10)]
plt.xticks(x, x_ticks)
plt.bar(x, y, color = 'g', width = 0.72, label = "Time")
plt.xlabel('n')
plt.ylabel('times')
plt.title('Time program for n with optimization flag: ' + flag)
plt.legend()
plt.show()