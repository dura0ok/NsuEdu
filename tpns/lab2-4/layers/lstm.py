import numpy as np

from .layer import Layer, BaseRecurrentLayer


class LSTM(Layer, BaseRecurrentLayer):
    def __init__(self, input_size, hidden_size, output_size, return_sequence=False, ws=0.2):
        super().__init__()
        self.input_size = input_size
        self.hidden_size = hidden_size
        self.output_size = output_size
        self.return_sequence = return_sequence
        self.ws = ws  # weights scale

        self.x = None
        self.hs = None
        self.cs = None
        self.os = None
        self.fs = None
        self.ins = None
        self._cs = None
        self.c_tanh = None

        if self.input_size is not None:
            self._init_weights()

    def _init_weights(self):
        # Weight matrices for input gate
        self.Wi = np.random.uniform(-self.ws, self.ws, (self.input_size, self.hidden_size))  # noqa
        self.Ui = np.random.uniform(-self.ws, self.ws, (self.hidden_size, self.hidden_size))
        self.bi = np.zeros(self.hidden_size)

        # Weight matrices for forget gate
        self.Wf = np.random.uniform(-self.ws, self.ws, (self.input_size, self.hidden_size))
        self.Uf = np.random.uniform(-self.ws, self.ws, (self.hidden_size, self.hidden_size))
        self.bf = np.zeros(self.hidden_size)

        # Weight matrices for output gate
        self.Wo = np.random.uniform(-self.ws, self.ws, (self.input_size, self.hidden_size))
        self.Uo = np.random.uniform(-self.ws, self.ws, (self.hidden_size, self.hidden_size))
        self.bo = np.zeros(self.hidden_size)

        # Weight matrices for cell state
        self.Wc = np.random.uniform(-self.ws, self.ws, (self.input_size, self.hidden_size))
        self.Uc = np.random.uniform(-self.ws, self.ws, (self.hidden_size, self.hidden_size))
        self.bc = np.zeros(self.hidden_size)

        # Weight matrices for output layer
        self.Wy = np.random.uniform(-self.ws, self.ws, (self.hidden_size, self.output_size))
        self.by = np.zeros(self.output_size)

    def forward(self, x):
        if self.input_size is None:
            self.input_size = x.shape[-1]
            self._init_weights()

        batch_size, sequence_length, _ = x.shape
        self.x = x

        # Initialize hidden states and cell state
        ht = np.zeros((batch_size, self.hidden_size))
        ct = np.zeros((batch_size, self.hidden_size))

        # Internal states for backprop
        self.hs = np.zeros((batch_size, sequence_length, self.hidden_size))
        self.cs = np.zeros((batch_size, sequence_length, self.hidden_size))
        self.ins = np.zeros((batch_size, sequence_length, self.hidden_size))
        self.os = np.zeros((batch_size, sequence_length, self.hidden_size))
        self.fs = np.zeros((batch_size, sequence_length, self.hidden_size))
        self._cs = np.zeros((batch_size, sequence_length, self.hidden_size))
        self.c_tanh = np.zeros((batch_size, sequence_length, self.hidden_size))

        for t in range(sequence_length):
            # Gates operations
            xt = x[:, t, :]
            it = self.sigmoid(xt @ self.Wi + ht @ self.Ui + self.bi)
            ft = self.sigmoid(xt @ self.Wf + ht @ self.Uf + self.bf)
            ot = self.sigmoid(xt @ self.Wo + ht @ self.Uo + self.bo)
            _c = self.tanh(xt @ self.Wc + ht @ self.Uc + self.bc)

            self.ins[:, t, :] = it
            self.os[:, t, :] = ot
            self.fs[:, t, :] = ft
            self._cs[:, t, :] = _c

            # Update and save cell state
            ct = ft * ct + it * _c
            self.cs[:, t, :] = ct

            # Update and save hidden state
            self.c_tanh[:, t, :] = self.tanh(ct)
            ht = ot * self.c_tanh[:, t, :]
            self.hs[:, t, :] = ht

        # Compute output
        if self.return_sequence:
            output = self.hs @ self.Wy + self.by
        else:
            final_hidden_state = self.hs[:, -1, :]
            output = final_hidden_state @ self.Wy + self.by

        return output

    def backward(self, dY, learning_rate):
        batch_size, sequence_length, _ = self.x.shape

        # Initialize gradients of weight matrices and biases
        dWi, dUi, dbi = np.zeros_like(self.Wi), np.zeros_like(self.Ui), np.zeros_like(self.bi)
        dWf, dUf, dbf = np.zeros_like(self.Wf), np.zeros_like(self.Uf), np.zeros_like(self.bf)
        dWo, dUo, dbo = np.zeros_like(self.Wo), np.zeros_like(self.Uo), np.zeros_like(self.bo)
        dWc, dUc, dbc = np.zeros_like(self.Wc), np.zeros_like(self.Uc), np.zeros_like(self.bc)
        dWy, dby = np.zeros_like(self.Wy), np.zeros_like(self.by)

        # Initialize gradients with respect to hidden states and cell states
        dh_next = np.zeros((batch_size, self.hidden_size))
        dc_next = np.zeros((batch_size, self.hidden_size))

        # Initialize the gradient with respect to the inputs
        dX = np.zeros((batch_size, sequence_length, self.input_size))

        for t in reversed(range(sequence_length)):
            dy = dY[:, t, :] if self.return_sequence else (dY if t == sequence_length - 1 else np.zeros_like(dY))

            # Compute gradients for output weights and bias
            dWy += self.hs[:, t, :].T @ dy
            dby += np.sum(dy, axis=0)

            # Gradient for hidden and cell states
            dh = dy @ self.Wy.T + dh_next
            dc = self.os[:, t, :] * dh * self.dtanh(self.cs[:, t, :]) + dc_next

            # Gradient for output gate
            dot = self.dsigmoid(self.os[:, t, :]) * self.c_tanh[:, t, :] * dh

            # Gradient for forget gate
            if t > 0:
                dft = self.cs[:, t-1, :] * dc * self.dsigmoid(self.fs[:, t, :])
            else:
                dft = np.zeros_like(self.fs[:, t, :])

            # Gradient for input gate
            dit = self._cs[:, t, :] * dc * self.dsigmoid(self.ins[:, t, :])

            # Gradient for cell state
            dct = self.ins[:, t, :] * dc * self.dtanh(self._cs[:, t, :])

            # Gate gradients
            dWi += self.x[:, t, :].T @ dit
            dbi += np.sum(dit, axis=0)

            dWf += self.x[:, t, :].T @ dft
            dbf += np.sum(dft, axis=0)

            dWo += self.x[:, t, :].T @ dot
            dbo += np.sum(dot, axis=0)

            dWc += self.x[:, t, :].T @ dct
            dbc += np.sum(dct, axis=0)

            if t > 0:
                dUi += self.hs[:, t - 1, :].T @ dit
                dUf += self.hs[:, t - 1, :].T @ dft
                dUo += self.hs[:, t - 1, :].T @ dot
                dUc += self.hs[:, t - 1, :].T @ dct

            # Next hidden state gradient
            dh_next = dit @ self.Ui.T + dft @ self.Uf.T + dot @ self.Uo.T + dct @ self.Uc.T
            dc_next = self.fs[:, t, :] * dc

            dX[:, t, :] = dit @ self.Wi.T + dft @ self.Wf.T + dot @ self.Wo.T + dct @ self.Wc.T

        # Update weights and biases
        self.Wi -= learning_rate * dWi / batch_size
        self.Ui -= learning_rate * dUi / batch_size
        self.bi -= learning_rate * dbi / batch_size
        self.Wf -= learning_rate * dWf / batch_size
        self.Uf -= learning_rate * dUf / batch_size
        self.bf -= learning_rate * dbf / batch_size
        self.Wo -= learning_rate * dWo / batch_size
        self.Uo -= learning_rate * dUo / batch_size
        self.bo -= learning_rate * dbo / batch_size
        self.Wc -= learning_rate * dWc / batch_size
        self.Uc -= learning_rate * dUc / batch_size
        self.bc -= learning_rate * dbc / batch_size
        self.Wy -= learning_rate * dWy / batch_size
        self.by -= learning_rate * dby / batch_size

        return dX

