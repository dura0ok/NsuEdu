from typing import Union, Tuple, Any, List, Dict

import numpy as np
import pandas as pd



def create_sequence_data(x_data, y_data, sequence_length=5):
    X_seq = []
    Y_seq = []

    for i in range(len(x_data) - sequence_length):
        X_seq.append(x_data[i:i + sequence_length])
        Y_seq.append(y_data[i + sequence_length])

    return np.array(X_seq), np.array(Y_seq)


def custom_train_test_split(x, y, test_size=0.2, random_state=None):
    if random_state is not None:
        np.random.seed(random_state)

    indices = np.arange(len(x))
    np.random.shuffle(indices)

    test_samples = int(len(x) * test_size)

    test_indices = indices[:test_samples]
    train_indices = indices[test_samples:]

    if isinstance(x, pd.DataFrame):
        x_train, x_test = x.iloc[train_indices], x.iloc[test_indices]
    elif isinstance(x, np.ndarray):
        x_train, x_test = x[train_indices], x[test_indices]
    else:
        raise ValueError("Unsupported type for 'x'. Use pandas DataFrame or NumPy array.")

    if isinstance(y, pd.Series):
        y_train, y_test = y.iloc[train_indices], y.iloc[test_indices]
    elif isinstance(y, np.ndarray):
        y_train, y_test = y[train_indices], y[test_indices]
    else:
        raise ValueError("Unsupported type for 'y'. Use pandas Series or NumPy array.")

    return x_train, x_test, y_train, y_test


def accuracy(y_true: np.ndarray, y_pred: np.ndarray) -> float:
    return np.mean(y_true == y_pred)


class DataProcessor:
    def __init__(self):
        self.mean = None
        self.std = None

    def normalize_data(self, data: np.ndarray) -> np.ndarray:
        self.mean = np.mean(data, axis=0)
        self.std = np.std(data, axis=0)
        normalized_data = (data - self.mean) / self.std
        return normalized_data

    def denormalize_data(self, normalized_data: np.ndarray) -> np.ndarray:
        if self.mean is None or self.std is None:
            raise ValueError("Mean and std values are not available. Please normalize the data first.")

        denormalized_data = (normalized_data * self.std) + self.mean
        return denormalized_data


def calculate_category_probabilities(x: pd.DataFrame, category_col: str = "category") -> List[float]:
    categories = x[category_col].unique()
    probabilities = [x[x[category_col] == category].shape[0] / x.shape[0] for category in categories]
    return probabilities


def calc_entropy(p: List[float]) -> float:
    return sum([-p_i * np.log2(p_i) if p_i != 0 else 0 for p_i in p])


def gain_ratio(dataset: pd.DataFrame, target_col: str = "target", category_col: str = "category") \
        -> Dict[Any, Tuple[float, float]]:
    x = dataset
    p = calculate_category_probabilities(x, category_col)
    h_x = calc_entropy(p)

    res = {}

    for column in dataset.columns:
        print(column)
        max_gain_ratio = float("-inf")
        if column == target_col or column == category_col:
            continue

        best_value = float("-inf")
        for value in dataset[column].unique():
            x_l = dataset.loc[dataset[column] <= value]
            x_r = dataset.loc[dataset[column] > value]

            p_l = calculate_category_probabilities(x_l, category_col)
            p_r = calculate_category_probabilities(x_r, category_col)

            h_x_l = calc_entropy(p_l)
            h_x_r = calc_entropy(p_r)

            gain_ratio_value = h_x - (x_l.shape[0] / x.shape[0]) * h_x_l - (x_r.shape[0] / x.shape[0]) * h_x_r
            if gain_ratio_value > max_gain_ratio:
                max_gain_ratio = gain_ratio_value
                best_value = value
                res[column] = (best_value, gain_ratio_value)

    return res


def calculate_mse(x: pd.Series) -> float:
    return np.mean((x - np.mean(x)) ** 2)


def gain_ratio_regression(dataset: pd.DataFrame, target_col: str = "target") \
        -> Dict[str, Tuple[float, float]]:
    res = {}
    target_mse = calculate_mse(dataset[target_col])

    for column in dataset.columns:
        if column == target_col:
            continue

        max_gain_ratio = float("-inf")
        best_value = None

        for value in dataset[column].unique():
            x_l = dataset.loc[dataset[column] <= value]
            x_r = dataset.loc[dataset[column] > value]

            if len(x_l) == 0 or len(x_r) == 0:
                continue

            mse_l = calculate_mse(x_l[target_col])
            mse_r = calculate_mse(x_r[target_col])

            weight_l = len(x_l) / len(dataset)
            weight_r = len(x_r) / len(dataset)

            weighted_mse = weight_l * mse_l + weight_r * mse_r
            gain_ratio_value = (target_mse - weighted_mse) / target_mse

            if gain_ratio_value > max_gain_ratio:
                max_gain_ratio = gain_ratio_value
                best_value = value

        res[column] = (best_value, max_gain_ratio)

    return res