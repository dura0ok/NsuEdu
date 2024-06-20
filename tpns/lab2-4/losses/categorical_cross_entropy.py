import numpy as np

from .loss import LossFunction


class CategoricalCrossEntropy(LossFunction):
    def __call__(self, y_true: np.ndarray, y_pred: np.ndarray) -> float:
        epsilon = 1e-15
        clipped_y_pred = np.clip(y_pred, epsilon, 1 - epsilon)
        return -np.sum(y_true * np.log(clipped_y_pred)) / y_pred.shape[0]

    def gradient(self, y_true: np.ndarray, y_pred: np.ndarray) -> np.ndarray:
        epsilon = 1e-15
        clipped_y_pred = np.clip(y_pred, epsilon, 1 - epsilon)
        return -(y_true - clipped_y_pred) / y_pred.shape[0]
