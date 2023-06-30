import subprocess

while True:
    out = subprocess.Popen(['./slau'], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    out.stdout.flush()
    result = out.stdout.readline().strip().decode("utf-8")
    if not result:
        print("Empty")
        continue

    iterations_count = int(result.split(":")[1].strip())
    print(iterations_count)
    if iterations_count > 2000:
        break
