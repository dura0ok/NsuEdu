import numpy as np

from .layer import Layer


class DenseLayer(Layer):
    def __init__(self, input_size=None, output_size=10, ws=0.05):
        self.ws = ws
        self.x = None
        self.weights = None
        self.output_size = output_size
        if input_size is not None:
            self.weights = np.random.randn(input_size, output_size) * self.ws
        self.bias = np.zeros((1, output_size))

    def forward(self, x):
        if self.weights is None:
            input_size = x.shape[-1]
            # print(f"Warning: Dense create weights with shape=({input_size, self.output_size})")
            self.weights = np.random.randn(input_size, self.output_size) * self.ws

        self.x = x
        output_data = np.dot(x, self.weights) + self.bias
        return output_data

    def backward(self, d_out, learning_rate):
        dx = np.dot(d_out, self.weights.T)
        dw = np.dot(self.x.T, d_out)
        db = np.sum(d_out, axis=0)
        self.weights -= learning_rate * dw
        self.bias -= learning_rate * db
        return dx
