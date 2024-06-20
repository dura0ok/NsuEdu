import numpy as np

from .activation import Activation


class Tanh(Activation):
    def __init__(self):
        self.output_data = None

    def forward(self, input_data: np.ndarray) -> np.ndarray:
        self.output_data = np.tanh(input_data)
        return self.output_data

    def backward(self, d_output: np.ndarray, **kwargs) -> np.ndarray:
        return d_output * (1 - np.square(self.output_data))
