from mlp import MLP
from layers.lstm import LSTM
from layers.dense import DenseLayer
from activations.relu import ReLU
from activations.softmax import Softmax
from losses.categorical_cross_entropy import CategoricalCrossEntropy

from energy_task import energy_task


def main():
    seed = 33

    drop_prev_load_type = True
    sequence_length = 6
    learning_rate = 0.03
    batch_size = 32
    epochs = 40

    ws = 0.3
    layers = [
        LSTM(input_size=None, hidden_size=64, output_size=44, return_sequence=True, ws=ws),
        ReLU(),
        LSTM(input_size=44, hidden_size=32, output_size=16, return_sequence=False, ws=ws),
        ReLU(),
        DenseLayer(16, 3, ws=ws),
        Softmax()
    ]

    model = MLP(layers, CategoricalCrossEntropy())

    energy_task(model, sequence_length, learning_rate, batch_size, epochs, drop_prev_load_type, seed=seed)


if __name__ == "__main__":
    main()
