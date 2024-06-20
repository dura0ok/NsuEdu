import numpy as np

from .layer import Layer, BaseRecurrentLayer


class GRU(Layer, BaseRecurrentLayer):
    def __init__(self, input_size, hidden_size, output_size, return_sequence=False, ws=0.2):
        super().__init__()
        self.input_size = input_size
        self.hidden_size = hidden_size
        self.output_size = output_size
        self.return_sequence = return_sequence
        self.ws = ws  # weights scale

        self.x = None
        self.z = None
        self.r = None
        self.h = None
        self.h_hat = None

        if self.input_size is not None:
            self._init_weights()

    def _init_weights(self):
        # Weight matrices for update gate
        self.Wz = np.random.uniform(-self.ws, self.ws, (self.input_size, self.hidden_size))  # noqa
        self.Uz = np.random.uniform(-self.ws, self.ws, (self.hidden_size, self.hidden_size))
        self.bz = np.zeros(self.hidden_size)

        # Weight matrices for reset gate
        self.Wr = np.random.uniform(-self.ws, self.ws, (self.input_size, self.hidden_size))
        self.Ur = np.random.uniform(-self.ws, self.ws, (self.hidden_size, self.hidden_size))
        self.br = np.zeros(self.hidden_size)

        # Weight matrices for candidate hidden state
        self.Wh = np.random.uniform(-self.ws, self.ws, (self.input_size, self.hidden_size))
        self.Uh = np.random.uniform(-self.ws, self.ws, (self.hidden_size, self.hidden_size))
        self.bh = np.zeros(self.hidden_size)

        # Weight matrices for output layer
        self.Wy = np.random.uniform(-self.ws, self.ws, (self.hidden_size, self.output_size))
        self.by = np.zeros(self.output_size)

    def forward(self, x):
        if self.input_size is None:
            self.input_size = x.shape[-1]
            self._init_weights()

        batch_size, sequence_length, _ = x.shape
        self.x = x

        h = np.zeros((batch_size, sequence_length, self.hidden_size))
        z = np.zeros((batch_size, sequence_length, self.hidden_size))
        r = np.zeros((batch_size, sequence_length, self.hidden_size))
        h_hat = np.zeros((batch_size, sequence_length, self.hidden_size))

        for t in range(sequence_length):
            prev_h = h[:, t-1, :] if t > 0 else np.zeros_like(h[:, 0, :])
            # Compute the update gate
            z[:, t, :] = self.sigmoid(x[:, t, :] @ self.Wz + prev_h @ self.Uz + self.bz)

            # Compute the reset gate
            r[:, t, :] = self.sigmoid(x[:, t, :] @ self.Wr + prev_h @ self.Ur + self.br)

            # Compute the candidate hidden state
            h_hat[:, t, :] = self.tanh(x[:, t, :] @ self.Wh + (r[:, t, :] * prev_h) @ self.Uh + self.bh)

            # Compute the current hidden state
            h[:, t, :] = z[:, t, :] * prev_h + (1 - z[:, t, :]) * h_hat[:, t, :]

        self.z = z
        self.r = r
        self.h = h
        self.h_hat = h_hat

        if self.return_sequence:
            o_t = self.h @ self.Wy + self.by
            return o_t

        o_t = self.h[:, -1, :] @ self.Wy + self.by
        return o_t

    def backward(self, dY, learning_rate=0.01):
        batch_size, sequence_length, _ = self.x.shape  # noqa

        # Initialize gradients
        dWz, dUz, dbz = np.zeros_like(self.Wz), np.zeros_like(self.Uz), np.zeros_like(self.bz)
        dWr, dUr, dbr = np.zeros_like(self.Wr), np.zeros_like(self.Ur), np.zeros_like(self.br)
        dWh, dUh, dbh = np.zeros_like(self.Wh), np.zeros_like(self.Uh), np.zeros_like(self.bh)
        dWy, dby = np.zeros_like(self.Wy), np.zeros_like(self.by)

        dh_next = np.zeros((batch_size, self.hidden_size))

        dX = np.zeros((batch_size, sequence_length, self.input_size))

        for t in reversed(range(sequence_length)):
            dy = dY[:, t, :] if self.return_sequence else (dY if t == sequence_length - 1 else np.zeros_like(dY))

            dWy += self.h[:, t, :].T @ dy
            dby += np.sum(dy, axis=0)

            # Intermediary derivatives
            dh = dy @ self.Wy.T + dh_next
            dh_hat = np.multiply(dh, (1 - self.z[:, t, :]))
            dh_hat_l = dh_hat * self.dtanh(self.h_hat[:, t, :])

            # ∂loss/∂Wh, ∂loss/∂Uh and ∂loss/∂bh
            dWh += np.dot(self.x[:, t, :].T, dh_hat_l)
            dUh += np.dot(np.multiply(self.r[:, t, :], self.h[:, t - 1, :]).T, dh_hat_l)
            dbh += np.sum(dh_hat_l, axis=0)

            # Intermediary derivatives
            drhp = np.dot(dh_hat_l, self.Uh.T)
            dr = np.multiply(drhp, self.h[:, t - 1, :])
            dr_l = dr * self.dsigmoid(self.r[:, t, :])

            # ∂loss/∂Wr, ∂loss/∂Ur and ∂loss/∂br
            dWr += np.dot(self.x[:, t, :].T, dr_l)
            dUr += np.dot(self.h[:, t - 1, :].T, dr_l)
            dbr += np.sum(dr_l, axis=0)

            # Intermediary derivatives
            dz = np.multiply(dh, self.h[:, t - 1, :] - self.h_hat[:, t, :])
            dz_l = dz * self.dsigmoid(self.z[:, t, :])

            # ∂loss/∂Wz, ∂loss/∂Uz and ∂loss/∂bz
            dWz += np.dot(self.x[:, t, :].T, dz_l)
            dUz += np.dot(self.h[:, t - 1, :].T, dz_l)
            dbz += np.sum(dz_l, axis=0)

            # All influences of previous layer to loss
            dh_fz_inner = np.dot(dz_l, self.Uz.T)
            dh_fz = np.multiply(dh, self.z[:, t, :])
            dh_fhh = np.multiply(drhp, self.r[:, t, :])
            dh_fr = np.dot(dr_l, self.Ur.T)

            # ∂loss/∂h𝑡₋₁
            dh_next = dh_fz_inner + dh_fz + dh_fhh + dh_fr

            dX[:, t, :] = dz_l @ self.Wz.T + dr_l @ self.Wr.T + dh_hat_l @ self.Wh.T

        # Update weights and biases
        self.Wz -= learning_rate * dWz / batch_size  # noqa
        self.Uz -= learning_rate * dUz / batch_size
        self.bz -= learning_rate * dbz / batch_size
        self.Wr -= learning_rate * dWr / batch_size
        self.Ur -= learning_rate * dUr / batch_size
        self.br -= learning_rate * dbr / batch_size
        self.Wh -= learning_rate * dWh / batch_size
        self.Uh -= learning_rate * dUh / batch_size
        self.bh -= learning_rate * dbh / batch_size
        self.Wy -= learning_rate * dWy / batch_size
        self.by -= learning_rate * dby / batch_size

        return dX
