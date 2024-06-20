import numpy as np

from .activation import Activation


class SigmoidActivation(Activation):
    def __init__(self):
        self.sigmoid_output = None

    def forward(self, input_data: np.ndarray) -> np.ndarray:
        self.sigmoid_output = 1 / (1 + np.exp(-input_data))
        return self.sigmoid_output

    def backward(self, d_output: np.ndarray, **kwargs) -> np.ndarray:
        return d_output * self.sigmoid_output * (1 - self.sigmoid_output)
