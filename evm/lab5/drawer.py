import json
from math import log2

from matplotlib import pyplot as plt

_suffixes = ['B', 'KB', 'MB', 'GB', 'TiB', 'PiB', 'EiB', 'ZiB', 'YiB']


def file_size(size):
    # determine binary order in steps of size 10
    # (coerce to int, // still returns a float)
    order = int(log2(size) / 10) if size else 0
    # format file size
    # (.4g results in rounded numbers for exact matches and max 3 decimals,
    # should never resort to exponent values)
    return '{:.4g}\n{}'.format(size / (1 << (order * 10)), _suffixes[order])


with open('data.json', 'r') as f:
    data = json.load(f)
    x = data['x']
    y = data['y']
    print(data)

    plt.plot(x["forward"], y["forward"], ls="dashed", label="прямой")
    plt.plot(x["backward"], y["backward"], ls="dotted", label="обратный")
    plt.plot(x["random"], y["random"], ls="solid", label="случайный")

    plt.xscale('log', base=2)
    nmin = 12
    nmax = 28
    xt = [2 ** i for i in range(nmin, nmax)]
    xt_label = [file_size(2 ** i) for i in range(nmin, nmax)]
    plt.xticks(xt, xt_label, rotation=45, fontsize=8)

    yt = [0, 50, 100, 150, 200]
    plt.yticks(yt)

    plt.xlabel("Размер массива")
    plt.ylabel("Время чтения элемента, такты")

    plt.legend()
    plt.tight_layout()

    # plt.show()
    plt.savefig('resermore.png', dpi=300)
