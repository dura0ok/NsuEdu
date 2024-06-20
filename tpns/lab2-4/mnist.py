import os
from pathlib import Path

import numpy as np
from scipy.ndimage import rotate
from sklearn.datasets import fetch_openml
from sklearn.preprocessing import OneHotEncoder

from helpers import custom_train_test_split
from mlp import MLP
from layers.average_pooling_layer import AvgPool2D
from layers.flatten import Flatten
from layers.conv2d import Conv2D
from layers.dense import DenseLayer
from activations.softmax import Softmax
from losses.categorical_cross_entropy import CategoricalCrossEntropy


def split_forward(model, x_data, batch_size=100):
    # Split data into batches
    num_samples = len(x_data)
    num_batches = (num_samples + batch_size - 1) // batch_size

    outputs = []

    for i in range(num_batches):
        start_index = i * batch_size
        end_index = min((i + 1) * batch_size, num_samples)
        batch_x = x_data[start_index:end_index]
        output = model.forward(batch_x)
        outputs.append(output)

    return np.concatenate(outputs, axis=0)


def main():
    # np.random.seed(33)

    mnist = fetch_openml("mnist_784", version=1, parser="auto")

    x, y = mnist.data, mnist.target

    x = x / 255.0

    x = x.values.reshape(-1, 1, 28, 28)
    x = np.pad(x, ((0, 0), (0, 0), (2, 2), (2, 2)), "constant", constant_values=0)

    y = y.values.astype(np.int8)
    encoder = OneHotEncoder(sparse_output=False, categories="auto")
    y_encoded = encoder.fit_transform(y.reshape(-1, 1))

    x_train, x_test, y_train, y_test = custom_train_test_split(x, y_encoded, test_size=0.2, random_state=42)
    layers = [
        Conv2D(in_channels=1, out_channels=6, kernel_size=5),
        AvgPool2D(pool_size=2, stride=2),
        Conv2D(in_channels=6, out_channels=16, kernel_size=5),
        AvgPool2D(pool_size=2, stride=2),
        Conv2D(in_channels=16, out_channels=120, kernel_size=5),
        Flatten(),
        DenseLayer(input_size=None, output_size=84),
        DenseLayer(input_size=84, output_size=10),
        Softmax()
    ]

    batch_size = 100
    model_file = Path(__file__).stem
    model: MLP | None = None

    if os.path.exists(model_file):
        model = MLP.load_model(model_file)
    else:
        model = MLP(layers, CategoricalCrossEntropy())
        model.train(x_train, y_train, epochs=20, batch_size=batch_size, learning_rate=0.005)
        model.save_model(model_file)

    # x_test = x_test.reshape(-1, 1, 1, 28, 28)
    # print("True:   ", np.argmax(y_test[25:55], axis=-1))
    # print("Predict:", np.argmax(model.forward(x_test[25:55]), axis=-1))
    y_pred = split_forward(model, x_test, batch_size=10)
    f1_score = model.calculate_f1_score(y_test, y_pred)
    print(f"F1 Score after test: {f1_score}")
    print(MLP.calculate_precision_recall(y_test, y_pred))
    model.print_precision_recall_table(y_test, y_pred)
    MLP.draw_roc_curve(y_test, y_pred)


if __name__ == "__main__":
    main()
