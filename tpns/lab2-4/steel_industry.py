import numpy as np
from sklearn.preprocessing import OneHotEncoder, MinMaxScaler, LabelEncoder

from activations.tanh import Tanh
from helpers import DataProcessor, custom_train_test_split
from layers.dense import DenseLayer
from layers.rnn import RNN
from losses.categorical_cross_entropy import CategoricalCrossEntropy
from mlp import MLP
from task3_loader import task3_loader


def main():
    features = ['NSM', 'Usage_kWh', 'CO2(tCO2)', 'Lagging_Current_Power_Factor', 'Lagging_Current_Reactive.Power_kVarh']
    target = "Load_Type"
    df = task3_loader()
    x = df[features]
    y = df[target]

    from numpy.lib.stride_tricks import sliding_window_view

    seq_len = 20
    target_size = 1

    def cut_df_to_dataset(df, seq_len, target_size, target_col):
        # features = df.drop(columns=[target_col]).to_numpy()
        # features = df[target_col].to_numpy()
        features = df.to_numpy()

        targets = df[target_col].to_numpy()

        x = sliding_window_view(features[0:-target_size], window_shape=(seq_len, 12))
        y = targets[seq_len:]
        x = np.squeeze(x, axis=1)
        return x.copy(), y.reshape(-1, 1).copy()
        # print(x.shape, y.shape)

    X_train, y_train = cut_df_to_dataset(df, seq_len, target_size, 'Usage_kWh')


    layers = [
        RNN(X_train.shape[2], 64),
        Tanh(),
        DenseLayer(64, 3)  # Add a DenseLayer after RNNLayer
    ]

    model = MLP(layers, CategoricalCrossEntropy())
    model.train(X_train, y_train, epochs=15000, learning_rate=0.0001)


if __name__ == '__main__':
    main()
