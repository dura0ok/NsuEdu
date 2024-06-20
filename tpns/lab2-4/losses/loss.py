from abc import ABC, abstractmethod

import numpy as np


class LossFunction(ABC):
    @abstractmethod
    def __call__(self, y_true: np.ndarray, y_pred: np.ndarray) -> float:
        pass

    @abstractmethod
    def gradient(self, y_true: np.ndarray, y_pred: np.ndarray) -> np.ndarray:
        pass
