import json
import subprocess

for i in range(1, 100):
    out = subprocess.Popen(['./cmake-build-debug/lab6'], stdout=subprocess.PIPE)
    data = out.stdout.readlines()
    try:
        with open('run2/data' + str(i) + '.json', 'w') as f:
            res = []
            for item in data:
                line = item.strip().decode("utf-8").split(" ")
                x = int(line[0])
                y = int(line[1])
                res.append([x, y])

            f.write(json.dumps(res))
    except Exception as e:
        print(e)
