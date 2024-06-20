from mlp import MLP
from layers.rnn import RNN
from layers.dense import DenseLayer
from activations.relu import ReLU
from activations.softmax import Softmax
from losses.categorical_cross_entropy import CategoricalCrossEntropy

from energy_task import energy_task


def main():
    seed = 33

    drop_prev_load_type = True
    sequence_length = 6
    learning_rate = 0.05
    batch_size = 32
    epochs = 25

    ws = 0.2
    layers = [
        RNN(input_size=None, hidden_size=24, output_size=24, return_sequence=True, ws=ws),
        ReLU(),
        RNN(input_size=24, hidden_size=12, output_size=10, return_sequence=False, ws=ws),
        ReLU(),
        DenseLayer(10, 3, ws=ws),
        Softmax()
    ]

    model = MLP(layers, CategoricalCrossEntropy())

    energy_task(model, sequence_length, learning_rate, batch_size, epochs, drop_prev_load_type, seed=seed)


if __name__ == "__main__":
    main()
