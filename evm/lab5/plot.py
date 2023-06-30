import json

from matplotlib import pyplot as plt
import subprocess
from math import log2

plt.figure(figsize=(8, 6))

_suffixes = ['B', 'KB', 'MB', 'GB', 'TiB', 'PiB', 'EiB', 'ZiB', 'YiB']


def file_size(size):
    # determine binary order in steps of size 10
    # (coerce to int, // still returns a float)
    order = int(log2(size) / 10) if size else 0
    # format file size
    # (.4g results in rounded numbers for exact matches and max 3 decimals,
    # should never resort to exponent values)
    return '{:.4g}\n{}'.format(size / (1 << (order * 10)), _suffixes[order])


x = {
    "forward": [],
    "backward": [],
    "random": []
}
y = {
    "forward": [],
    "backward": [],
    "random": []
}

n = 10
out = subprocess.Popen(['./cmake-build-debug/lab5', str(n)], stdout=subprocess.PIPE)
data = out.stdout.readlines()
for item in data:
    line = item.strip().decode("utf-8").split(",")
    try:
        x[line[0]].append(int(line[1]))
        y[line[0]].append(float(line[3]))
    except Exception as e:
        print(e)

with open('data.json', 'w') as f:
    data = {"x": x, "y": y}
    f.write(json.dumps(data))


# plt.plot(x["forward"], y["forward"], ls="dashed", label="прямой")
# plt.plot(x["backward"], y["backward"], ls="dotted", label="обратный")
# plt.plot(x["random"], y["random"], ls="solid", label="случайный")
#
# xticks_a= [32*1024, 256*1024, 2048*1024]
# yticks_a = [0, 50, 100,  150, 200]
# plt.xticks(xticks_a,  ['32 KB', '256 KB', '2048 KB'], rotation = 45, fontsize=8 )
# plt.yticks(yticks_a)
#
# plt.xlabel("Размер массива")
# plt.ylabel("Время чтения элемента, такты")
#
# plt.legend()
#
# plt.savefig('res.png', dpi=300)
