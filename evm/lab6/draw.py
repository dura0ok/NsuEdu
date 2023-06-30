from matplotlib import pyplot as plt
import json

for i in range(1, 100):
	plt.figure(figsize=(8, 6))

	data = json.load(open("run2/data" + str(i) + ".json"))
	data = sorted(data, key=lambda el: el[0])

	x = list([a[0] for a in data])
	y = list([a[1] for a in data])


	plt.plot(x,y)
	plt.scatter(x, y)

	xticks = []


	plt.xticks(x)

	plt.xlabel("Число фрагментов")
	plt.ylabel("Время чтения элемента, такты")

	plt.savefig('run2/output/' + str(i) + '.png', dpi=300)
	plt.close()
