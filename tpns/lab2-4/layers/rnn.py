import numpy as np

from .layer import Layer, BaseRecurrentLayer


class RNN(Layer, BaseRecurrentLayer):
    def __init__(self, input_size, hidden_size, output_size, return_sequence=False, ws=0.2):
        self.input_size = input_size
        self.hidden_size = hidden_size
        self.output_size = output_size
        self.return_sequence = return_sequence
        self.ws = ws  # weights scale

        self.input_data = None
        self.hiddens = None

        # self.layers = None
        self.input_weights = None
        self.hidden_weights = None
        self.output_weights = None
        self.hidden_bias = None
        self.output_bias = None

        if self.input_size is not None:
            self._init_weights()

    def _init_weights(self):
        self.input_weights = np.random.uniform(-self.ws, self.ws, size=(self.input_size, self.hidden_size))
        self.hidden_weights = np.random.uniform(-self.ws, self.ws, size=(self.hidden_size, self.hidden_size))
        self.output_weights = np.random.uniform(-self.ws, self.ws, size=(self.hidden_size, self.output_size))
        self.hidden_bias = np.zeros(self.hidden_size)
        self.output_bias = np.zeros(self.output_size)

    def forward(self, input_data):
        if self.input_size is None:
            self.input_size = input_data.shape[-1]
            self._init_weights()

        self.input_data = input_data
        batch_size, sequence_length, _ = input_data.shape
        hidden = np.zeros((batch_size, self.hidden_size))
        self.hiddens = np.zeros((batch_size, sequence_length, self.hidden_size))

        input_x = np.dot(input_data, self.input_weights)
        for i in range(sequence_length):
            hidden = input_x[:, i, :] + np.dot(hidden, self.hidden_weights) + self.hidden_bias
            hidden = self.tanh(hidden)
            self.hiddens[:, i, :] = hidden

        if self.return_sequence:
            output = np.dot(self.hiddens, self.output_weights) + self.output_bias
        else:
            final_hidden_state = self.hiddens[:, -1, :]
            output = np.dot(final_hidden_state, self.output_weights) + self.output_bias

        return output

    def backward(self, d_out, learning_rate):
        batch_size, sequence_length, _ = self.input_data.shape

        # Initialize gradients
        input_weights_gradient = np.zeros_like(self.input_weights)
        hidden_weights_gradient = np.zeros_like(self.hidden_weights)
        output_weights_gradient = np.zeros_like(self.output_weights)
        output_bias_gradient = np.zeros_like(self.output_bias)
        hidden_bias_gradient = np.zeros_like(self.hidden_bias)

        hidden_error_gradient = np.zeros((batch_size, self.hidden_size))

        d_in = np.zeros_like(self.input_data)

        for i in reversed(range(sequence_length)):
            # Compute output && hidden layers gradients
            hidden_i = self.hiddens[:, i, :]

            if self.return_sequence:
                l_grad_i = d_out[:, i, :]
                output_weights_gradient += np.dot(hidden_i.T, l_grad_i)
                output_bias_gradient += np.mean(l_grad_i, axis=0)

                hidden_error = np.dot(l_grad_i, self.output_weights.T) + hidden_error_gradient
            else:
                if i == sequence_length - 1:
                    l_grad_i = d_out
                    output_weights_gradient += np.dot(hidden_i.T, l_grad_i)
                    output_bias_gradient += np.mean(l_grad_i, axis=0)

                    hidden_error = np.dot(l_grad_i, self.output_weights.T)
                else:
                    hidden_error = hidden_error_gradient

            hidden_derivative = self.dtanh(hidden_i)
            h_grad_i = hidden_derivative * hidden_error

            if i > 0:
                hidden_weights_gradient += np.dot(self.hiddens[:, i - 1, :].T, h_grad_i)
                hidden_bias_gradient += np.mean(h_grad_i, axis=0)

            # Compute input layer gradients
            input_x = self.input_data[:, i, :]
            input_weights_gradient += np.dot(input_x.T, h_grad_i)

            # Update hidden error gradient
            hidden_error_gradient = np.dot(h_grad_i, self.hidden_weights.T)

            d_in[:, i, :] = np.dot(h_grad_i, self.input_weights.T)

        # update weights
        self.input_weights -= learning_rate * input_weights_gradient / batch_size
        self.hidden_weights -= learning_rate * hidden_weights_gradient / batch_size
        self.output_weights -= learning_rate * output_weights_gradient / batch_size
        self.hidden_bias -= learning_rate * hidden_bias_gradient / batch_size
        self.output_bias -= learning_rate * output_bias_gradient / batch_size

        return d_in

