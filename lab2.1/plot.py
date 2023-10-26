# Parse file
with open('out.txt', 'r', encoding='utf-8') as f:
    lines = f.readlines()

data = []
for line in lines:
    data.append(line[:-1].split(' '))
for i in range(0, len(data)):
    data[i] = [int(x) for x in data[i]]

MAX_COORD = data[0][0]
POINT_COUNT = data[0][1]
k = data[1][0]
data = data[2:]

# Кластеры до изменений
old_clasters = []
i = 0
for claster_id in range(0, k):
    old_clasters.append([data[i][2], data[i][3]])
    claster_size = data[i][1]
    i += 1
    points = []
    j = 0
    while (j < claster_size):
        points.append(data[i])
        j += 1
        i += 1
    old_clasters[claster_id].append(points)

# print(old_clasters[0])
# print(old_clasters[1])

# Кластеры после изменений
data = data[i:]

new_clasters = []
i = 0
for claster_id in range(0, k):
    new_clasters.append([data[i][2], data[i][3]])
    claster_size = data[i][1]
    i += 1
    points = []
    j = 0
    while (j < claster_size):
        points.append(data[i])
        j += 1
        i += 1
    new_clasters[claster_id].append(points)
# print("---------------------")
# print(new_clasters[0])
# print(new_clasters[1])

# Строим графики
# import numpy as np
import matplotlib.pyplot as plt
 
fig = plt.figure(figsize=(4, 4))
fig_2 = plt.figure(figsize=(4, 4))
ax_1 = fig.add_subplot()
ax_2 = fig_2.add_subplot()

colors = ['r', 'g', 'b', 'c', 'm', 'y', 'k', 'w']
i = 0
for old_claster in old_clasters:
    x = []
    y = []
    for elem in old_claster[2]:
        x.append(elem[0])
        y.append(elem[1])
    ax_1.scatter(x, y, color = colors[i])
    ax_1.scatter(old_claster[0], old_claster[1], color = colors[i], marker = 's')
    i += 1

i = 0
for new_claster in new_clasters:
    x = []
    y = []
    for elem in new_claster[2]:
        x.append(elem[0])
        y.append(elem[1])
    ax_2.scatter(x, y, color = colors[i])
    ax_2.scatter(new_claster[0], new_claster[1], color = colors[i], marker = 's')
    i += 1


ax_1.grid()
ax_2.grid()
plt.show()
