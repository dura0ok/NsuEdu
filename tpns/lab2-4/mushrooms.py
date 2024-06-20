import os
from pathlib import Path

import numpy as np
from sklearn.preprocessing import OneHotEncoder
from ucimlrepo import fetch_ucirepo

from activations.relu import ReLU
from activations.softmax import Softmax
from helpers import custom_train_test_split
from layers.dense import DenseLayer
from losses.categorical_cross_entropy import CategoricalCrossEntropy

from mlp import MLP
from sklearn import metrics


def main():
    mushroom = fetch_ucirepo(id=73)
    features = mushroom.data.features
    target = mushroom.data.targets

    selected_categorical_features = ['gill-color', 'gill-size', 'ring-type']
    x = features[selected_categorical_features]

    encoder = OneHotEncoder()
    x_encoded = encoder.fit_transform(x).toarray()
    y_encoded = encoder.fit_transform(target).toarray()

    x_train, x_test, y_train, y_test = custom_train_test_split(x_encoded, y_encoded, test_size=0.2, random_state=42)

    input_size = x_train.shape[1]
    output_size = len(np.unique(target))
    layers = [
        DenseLayer(input_size, 16),
        ReLU(),
        DenseLayer(16, 8),
        ReLU(),
        DenseLayer(8, output_size),
        Softmax()
    ]

    model_file = Path(__file__).stem

    model: MLP | None = None
    if os.path.exists(model_file):
        model = MLP.load_model(model_file)
    else:
        model = MLP(layers, CategoricalCrossEntropy())
        model.train(x_train, y_train, epochs=10000, learning_rate=0.01)
        model.save_model(model_file)

    print(f"F1 Score after train: {model.calculate_f1_score(y_train, model.forward(x_train))}")
    print(f"F1 Score after test: {model.calculate_f1_score(y_test, model.forward(x_test))}")
    print(x_test[0], encoder.inverse_transform(model.predict(x_test[5])), encoder.inverse_transform([y_test[5]]))
    print(MLP.calculate_precision_recall(y_test, model.forward(x_test)))
    MLP.draw_roc_curve(y_test, model.forward(x_test))


if __name__ == '__main__':
    main()
