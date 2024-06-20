import numpy as np
from .layer import Layer


class AvgPool2D(Layer):
    def __init__(self, pool_size=2, stride=2):
        self.cache = None
        self.pool_size = pool_size
        self.stride = stride

    def forward(self, x):
        n, c, h, w = x.shape

        pad_h = (self.pool_size - (h % self.pool_size)) % self.pool_size
        pad_w = (self.pool_size - (w % self.pool_size)) % self.pool_size

        # Pad the input on the bottom and right if necessary
        if pad_h > 0 or pad_w > 0:
            x_padded = np.pad(x, ((0, 0), (0, 0), (0, pad_h), (0, pad_w)), mode='constant', constant_values=0)
            print(x.shape, x_padded.shape)
        else:
            x_padded = x

        n, c, h_padded, w_padded = x_padded.shape

        x_reshaped = x_padded.reshape(n, c, h_padded // self.pool_size, self.pool_size, w_padded // self.pool_size,
                                      self.pool_size)
        x_strided = x_reshaped.transpose(0, 1, 2, 4, 3, 5).reshape(n, c, h_padded // self.pool_size,
                                                                   w_padded // self.pool_size,
                                                                   self.pool_size * self.pool_size)

        # Perform average pooling
        out = x_strided.mean(axis=4)
        self.cache = (x.shape, x_strided.shape, out.shape, (pad_h, pad_w))  # Include padding in cache for backward
        return out

    def backward(self, dout, **kwargs):
        n, c, h, w = self.cache[0]
        _, _, out_h, out_w, _ = self.cache[1]
        pad_h, pad_w = self.cache[3]

        dx_strided = np.zeros((n, c, out_h, out_w, self.pool_size * self.pool_size))

        # Distribute gradient evenly to each element in the pooling window
        dout_reshaped = dout[:, :, :, :, np.newaxis]
        dx_strided[:] = dout_reshaped / (self.pool_size * self.pool_size)

        # Reshape gradient back to match the input shape
        dx_strided = dx_strided.reshape(n, c, out_h, self.pool_size, out_w, self.pool_size).transpose(0, 1, 2, 4, 3, 5)
        dx = dx_strided.reshape(n, c, h + pad_h, w + pad_w)

        # Remove padding from the gradient if it was added
        if pad_h > 0 or pad_w > 0:
            dx = dx[:, :, :h, :w]

        return dx
