import numpy as np
import matplotlib.pyplot as plt
 
# data to be plotted
x = [2, 4, 8, 16, 24]
y1 = [38.47, 21.28, 12.91, 13.46, 11.25]
y2 = [43.41, 23.85, 14.21, 14.62, 11.98]
 
# plotting
plt.title("Время работы при различных n(красная точка точка, синяя коллективное)")
plt.xlabel("n")
plt.ylabel("Время работы программы")
plt.plot(x, y1, color ="red")
plt.plot(x, y2, color ="blue")
plt.show()