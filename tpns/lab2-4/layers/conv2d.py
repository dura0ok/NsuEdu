import numpy as np

from .layer import Layer


class Conv2D(Layer):
    def __init__(self, in_channels, out_channels, kernel_size, stride=1, pad=0):
        self.x = None
        self.col = None
        self.in_channels = in_channels
        self.out_channels = out_channels
        self.kernel_size = kernel_size
        self.stride = stride
        self.pad = pad
        self.weights = np.random.randn(out_channels, in_channels, kernel_size, kernel_size) * 0.05
        self.bias = np.random.randn(out_channels) * 0.05

    def forward(self, x):
        self.x = x
        self.col = self.im2col(x, self.kernel_size, self.stride, self.pad)
        col_w = self.weights.reshape(self.out_channels, -1).T
        out = np.dot(self.col, col_w) + self.bias
        n, c, h, w = x.shape
        out_h = (h + 2 * self.pad - self.kernel_size) // self.stride + 1
        out_w = (w + 2 * self.pad - self.kernel_size) // self.stride + 1
        out = out.reshape(n, out_h, out_w, -1).transpose(0, 3, 1, 2)
        return out

    def backward(self, dout, learning_rate=0.01):
        dout = dout.transpose(0, 2, 3, 1).reshape(-1, self.out_channels)
        db = np.sum(dout, axis=0)
        dw = np.dot(self.col.T, dout)
        dw = dw.transpose(1, 0).reshape(self.out_channels, self.in_channels, self.kernel_size, self.kernel_size)
        self.weights -= learning_rate * dw
        self.bias -= learning_rate * db
        d_col = np.dot(dout, self.weights.reshape(self.out_channels, -1))
        dx = self.col2im(d_col, self.x.shape, self.kernel_size, self.stride, self.pad)
        return dx

    @staticmethod
    def im2col(input_data, kernel_size, stride, pad):
        n, c, h, w = input_data.shape

        out_h = (h + 2 * pad - kernel_size) // stride + 1
        out_w = (w + 2 * pad - kernel_size) // stride + 1

        img = input_data
        col = np.zeros((n, c, kernel_size, kernel_size, out_h, out_w))
        for y in range(kernel_size):
            y_max = y + stride * out_h
            for x in range(kernel_size):
                x_max = x + stride * out_w
                col[:, :, y, x, :, :] = img[:, :, y:y_max:stride, x:x_max:stride]
        col = col.transpose(0, 4, 5, 1, 2, 3).reshape(n * out_h * out_w, -1)
        return col

    @staticmethod
    def col2im(cols, input_shape, kernel_size, stride, pad):
        n, c, h, w = input_shape
        h += 2 * pad
        w += 2 * pad
        h_padded, d_col = h, w
        img_padded = np.zeros((n, c, h_padded, d_col), dtype=cols.dtype)
        out_h = (h - kernel_size) // stride + 1
        out_w = (w - kernel_size) // stride + 1
        cols_reshaped = cols.reshape(n, out_h, out_w, c, kernel_size, kernel_size).transpose(0, 3, 4, 5, 1, 2)
        for y in range(kernel_size):
            y_max = y + stride * out_h
            for x in range(kernel_size):
                x_max = x + stride * out_w
                img_padded[:, :, y:y_max:stride, x:x_max:stride] += cols_reshaped[:, :, y, x, :, :]
        if pad > 0:
            return img_padded[:, :, pad:-pad, pad:-pad]
        return img_padded
