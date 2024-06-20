import pickle

import numpy as np
from matplotlib import pyplot as plt
from tqdm import tqdm

from layers.layer import Layer
from losses.loss import LossFunction


class MLP:
    def __init__(self, layers: list[Layer], loss_function: LossFunction):
        self.layers = layers
        self.loss_function = loss_function

    def forward(self, input_data: np.ndarray) -> np.ndarray:
        for layer in self.layers:
            input_data = layer.forward(input_data)
        return input_data

    def backward(self, d_output: np.ndarray, learning_rate) -> None:
        for layer in reversed(self.layers):
            d_output = layer.backward(d_output, learning_rate=learning_rate)

    @staticmethod
    def calculate_r2_score(y_true, y_pred):
        mean_true = np.mean(y_true)
        ss_total = np.sum((y_true - mean_true) ** 2)
        ss_res = np.sum((y_true - y_pred) ** 2)
        return 1 - (ss_res / ss_total)

    @staticmethod
    def calculate_precision_recall(y_true: np.ndarray, y_pred: np.ndarray) -> tuple:
        num_classes = y_true.shape[1]
        precisions = []
        recalls = []

        for class_index in range(num_classes):
            true_positive = np.sum((y_true[:, class_index] == 1) & (np.argmax(y_pred, axis=1) == class_index))
            predicted_positive = np.sum(np.argmax(y_pred, axis=1) == class_index)
            actual_positive = np.sum(y_true[:, class_index] == 1)

            if predicted_positive == 0:
                precisions.append(0)
            else:
                precisions.append(true_positive / predicted_positive)

            if actual_positive == 0:
                recalls.append(0)
            else:
                recalls.append(true_positive / actual_positive)

        return precisions, recalls

    @staticmethod
    def print_precision_recall_table(y_true, y_pred, target_names=None):
        precisions, recalls = MLP.calculate_precision_recall(y_true, y_pred)
        if target_names is None:
            target_names = [f'Class {i}' for i in range(len(precisions))]

        print("{:<20} {:<10} {:<10}".format("Class", "Precision", "Recall"))
        print("-" * 40)
        for i, class_name in enumerate(target_names):
            print("{:<20} {:<10.4f} {:<10.4f}".format(class_name, precisions[i], recalls[i]))

    @staticmethod
    def calculate_f1_score(y_true: np.ndarray, y_pred: np.ndarray) -> float:
        precisions, recalls = MLP.calculate_precision_recall(y_true, y_pred)
        f1_scores = []
        for precision, recall in zip(precisions, recalls):
            if precision + recall == 0:
                f1_scores.append(0)
            else:
                f1_scores.append(2 * (precision * recall) / (precision + recall))
        return np.mean(f1_scores)

    @staticmethod
    def draw_roc_curve(y_true: np.ndarray, y_pred: np.ndarray):
        num_classes = y_true.shape[1]
        all_fpr = []
        all_tpr = []

        plt.figure()
        for class_index in range(num_classes):
            fpr, tpr = MLP.calculate_roc_values(y_true[:, class_index], y_pred[:, class_index])
            roc_auc = MLP.auc(fpr, tpr)
            plt.plot(fpr, tpr, label='Class {} (AUC = {:.2f})'.format(class_index, roc_auc))
            interpolated_tpr = np.interp(np.linspace(0, 1, 100), fpr, tpr)
            all_fpr.append(np.linspace(0, 1, 100))
            all_tpr.append(interpolated_tpr)

        mean_tpr = np.mean(all_tpr, axis=0)
        mean_fpr = np.linspace(0, 1, 100)

        plt.plot(mean_fpr, mean_tpr, label='Micro-average ROC', color='deeppink', linestyle='--')

        plt.plot([0, 1], [0, 1], color='navy', linestyle='--')
        plt.xlim([0.0, 1.0])
        plt.ylim([0.0, 1.05])
        plt.xlabel('False Positive Rate')
        plt.ylabel('True Positive Rate')
        plt.title('ROC')
        plt.legend(loc="lower right")
        plt.show()

    @staticmethod
    def calculate_roc_values(y_true, y_pred):
        threshold_values = sorted(set(y_pred), reverse=True)
        tpr_values = [0]
        fpr_values = [0]
        num_positive = np.sum(y_true)
        num_negative = len(y_true) - num_positive

        for threshold in threshold_values:
            y_pred_binary = np.where(y_pred >= threshold, 1, 0)
            true_positives = np.sum((y_pred_binary == 1) & (y_true == 1))
            false_positives = np.sum((y_pred_binary == 1) & (y_true == 0))
            tpr = true_positives / num_positive
            fpr = false_positives / num_negative
            tpr_values.append(tpr)
            fpr_values.append(fpr)

        return fpr_values, tpr_values

    @staticmethod
    def auc(fpr, tpr):
        auc_value = 0
        for i in range(1, len(fpr)):
            auc_value += (fpr[i] - fpr[i - 1]) * tpr[i]
        return auc_value

    def train(self, x_train_data: np.ndarray, y_train_data: np.ndarray, epochs: int, learning_rate: float,
              batch_size: int = 50) -> None:
        progress_bar = tqdm(total=epochs, desc="Training Progress")

        data_size = len(x_train_data)
        for epoch in range(epochs):
            indices = np.random.permutation(data_size)
            x_train_data_shuffled = x_train_data[indices]
            y_train_data_shuffled = y_train_data[indices]

            mean_loss_train = 0.0
            for batch_start in range(0, data_size, batch_size):
                batch_end = min(batch_start + batch_size, data_size)
                x_batch = x_train_data_shuffled[batch_start:batch_end]
                y_batch = y_train_data_shuffled[batch_start:batch_end]

                predictions_train = self.forward(x_batch)
                mean_loss_train += self.loss_function(y_batch, predictions_train)
                d_output_train = self.loss_function.gradient(y_batch, predictions_train)
                self.backward(d_output_train, learning_rate)

            progress_bar.update(1)
            progress_bar.set_postfix({"Epoch": epoch + 1, "Train Mean Loss": f"{mean_loss_train / (data_size/batch_size):.4f}"})

    def predict(self, x_test: np.ndarray) -> np.ndarray:
        return self.forward(x_test)

    def save_model(self, filename):
        with open(filename, 'wb') as f:
            pickle.dump(self, f)

    @staticmethod
    def load_model(filename):
        with open(filename, 'rb') as f:
            return pickle.load(f)
