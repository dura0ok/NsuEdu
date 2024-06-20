import numpy as np

from .loss import LossFunction


class MeanSquaredError(LossFunction):
    def __call__(self, y_true: np.ndarray, y_pred: np.ndarray) -> float:
        return np.mean((y_true - y_pred) ** 2)

    def gradient(self, y_true: np.ndarray, y_pred: np.ndarray) -> np.ndarray:
        return 2 * (y_pred - y_true) / len(y_true)
