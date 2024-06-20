import numpy as np

from .activation import Activation


class Softmax(Activation):
    def __init__(self):
        self.output_data = None

    def forward(self, input_data: np.ndarray) -> np.ndarray:
        exp_values = np.exp(input_data - np.max(input_data, axis=-1, keepdims=True))
        probabilities = exp_values / np.sum(exp_values, axis=-1, keepdims=True)
        self.output_data = probabilities
        return self.output_data

    def backward(self, d_output: np.ndarray, **kwargs) -> np.ndarray:
        return d_output
