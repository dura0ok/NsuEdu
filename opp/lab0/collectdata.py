import subprocess
import time

start = time.time()
out = subprocess.Popen(["./dot1"], stdout=subprocess.PIPE)
tmp = out.stdout.readlines()
print("--- %s seconds ---" % (time.time() - start))

for j in [2, 4, 8, 16, 24]:
	print("Starting with n: " + str(j))
	for i in range(2, 4):
		start_time = time.time()
		out = subprocess.Popen(["mpirun", "-n", str(j), "./dot" + str(i)], stdout=subprocess.PIPE)
		data = out.stdout.readlines()
		print("--- %s seconds ---" % (time.time() - start_time))

	print("===========================")
	print("===========================")
	print("===========================")
