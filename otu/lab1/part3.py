import numpy as np

x_values = np.array([0, 1.5, 3])
y_values = np.array([0.85, 0.55, 0.45])

ln_y_values = np.log(y_values)

coefficients = np.polyfit(x_values, ln_y_values, 1)

a = np.exp(coefficients[1])
b = coefficients[0]

print("Коэффициент a:", a)
print("Коэффициент b:", b)

def exponential_function(x):
    return a * np.exp(b * x)

x_example = 2
print("Для x =", x_example, "аппроксимированное значение y:", exponential_function(x_example))
