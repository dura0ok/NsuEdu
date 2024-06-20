import pandas as pd

from activations.relu import ReLU
from helpers import custom_train_test_split, DataProcessor
from layers.dense import DenseLayer
from losses.mse import MeanSquaredError
from mlp import MLP


def main():
    data = pd.read_csv('data.csv')
    features = data[['Storage_Capacity', 'RAM_Size']].values
    target = data['Price'].values.reshape(-1, 1)

    dp = DataProcessor()
    features = dp.normalize_data(features)
    target = dp.normalize_data(target)

    x_train, x_test, y_train, y_test = custom_train_test_split(features, target, test_size=0.2, random_state=42)

    input_size = x_train.shape[1]
    output_size = 1

    layers = [
        DenseLayer(input_size, 64),
        ReLU(),
        DenseLayer(64, output_size)
    ]
    model = MLP(layers, MeanSquaredError())
    model.train(x_train, y_train, epochs=15000, learning_rate=0.0001)
    print(
        f"Real answer - {dp.denormalize_data(x_test[0])[0]}, "
        f"Predict: {dp.denormalize_data(model.forward(x_test[0]))[0][0]}"
    )
    print(f"R2 Score after train: {model.calculate_r2_score(y_train, model.forward(x_train))}")
    print(f"R2 Score after train: {model.calculate_r2_score(y_test, model.forward(x_test))}")
    print(MeanSquaredError()(y_test, model.forward(x_test)))
    MLP.draw_roc_curve(y_test, model.forward(x_test))


if __name__ == '__main__':
    main()
