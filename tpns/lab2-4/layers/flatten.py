import numpy as np

from .layer import Layer


class Flatten(Layer):
    def __init__(self):
        self.input_shape = None

    def forward(self, input_data: np.ndarray) -> np.ndarray:
        self.input_shape = input_data.shape
        return input_data.reshape(input_data.shape[0], -1)

    def backward(self, d_output: np.ndarray, **kwargs) -> np.ndarray:
        return d_output.reshape(self.input_shape)
