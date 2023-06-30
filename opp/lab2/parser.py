import os
files = os.listdir("out")
for file in files:
	splitted = file.split("sh")
	splitted = list(filter(None, splitted))
	if len(splitted) == 2 and not "e" in splitted[1]:
		with open("out/" + file, "r") as f:
			lines = []
			for line in f:
				lines.append(line)

			iterations_count = int(lines[2].split(" ")[2])
			third = lines[3].split(" ")
			abs_time = float(third[2])
			rel_time = float(third[6])

			name = file.split("sh")
			name = name[0].replace(".", "")

			print(name, iterations_count, abs_time, rel_time)




