from abc import ABC, abstractmethod

import numpy as np


class Activation(ABC):
    @abstractmethod
    def forward(self, input_data: np.ndarray) -> np.ndarray:
        pass

    @abstractmethod
    def backward(self, d_output: np.ndarray, **kwargs) -> np.ndarray:
        pass
