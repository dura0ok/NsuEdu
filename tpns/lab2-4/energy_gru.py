from mlp import MLP
from layers.gru import GRU
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
    epochs = 35

    ws = 0.3
    layers = [
        GRU(input_size=None, hidden_size=32, output_size=32, return_sequence=True, ws=ws),
        ReLU(),
        GRU(input_size=32, hidden_size=16, output_size=12, return_sequence=False, ws=ws),
        ReLU(),
        DenseLayer(12, 3, ws=ws),
        Softmax()
    ]

    model = MLP(layers, CategoricalCrossEntropy())

    energy_task(model, sequence_length, learning_rate, batch_size, epochs, drop_prev_load_type, seed=seed)


if __name__ == "__main__":
    main()
