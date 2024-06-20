from abc import abstractmethod, ABC

import numpy as np


class Layer(ABC):
    @abstractmethod
    def forward(self, input_data: np.ndarray) -> np.ndarray:
        pass

    @abstractmethod
    def backward(self, d_output: np.ndarray, learning_rate) -> np.ndarray:
        pass


class BaseRecurrentLayer:
    @staticmethod
    def sigmoid(x):
        return 1 / (1 + np.exp(-x))

    @staticmethod
    def dsigmoid(x):
        return x * (1 - x)

    @staticmethod
    def tanh(x):
        return 2 / (1 + np.exp(-2 * x)) - 1

    @staticmethod
    def dtanh(x):
        return 1 - x ** 2
