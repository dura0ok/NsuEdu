import os


import subprocess
#nums = [1] + [i for i in range(2, 1000, 2)]
nums = [1, 2, 4, 6, 8, 12, 12, 16, 32, 60, 120, 250, 500, 1000]

for type in ["static", "dynamic", "guided"]:
	for i in nums:
		lines = []
		with open('backup2.c', 'r') as input_file:
			for line in input_file:
				line = line.replace("#pragma omp for", "#pragma omp for schedule"+ "(" + type + ", " + str(i) + ")")
				lines.append(line)


		name = type + str(i)
		with open("out/" + name + ".c", 'w') as f:
		    f.writelines(lines)


		bash_lines = []
		with open('example.sh', 'r') as input_file:
			for line in input_file:
				line = line.replace("example", name)
				bash_lines.append(line)

		with open("out/" + name + ".sh", 'w') as f:
		    f.writelines(bash_lines)


		os.system("cd out && qsub " + name + ".sh")



