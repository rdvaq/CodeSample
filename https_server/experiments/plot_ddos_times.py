import numpy as np
import matplotlib.pyplot as plt
import os


# demonstrate that if we create a client that spawns a bunch of connections, they begin to significantly slow down the proxy
data = [
    (100, [0.01, 0.01, 0.01, 0.01, 0.02]),
    (200, [0.01, 19.44, 2.05, 20.47, 13.31]),
    (300, [20.49, 38.91, 10.23, 12.29, 19.44]),
    (400, [23.59, 20.48, 17.42, 15.34, 11.27]),
    (500, [27.67, 23.57, 28.68, 18.42, 58.37]),
    (600, [44.03, 34.86, 41.97, 72.71, 45.06]),
    (700, [60.41, 63.49, 60.44, 42.0, 61.43])
]

sizes = list()
times = list()
for e in data:
    sizes.append(e[0])
    times.append(e[1])
sizes = np.array(sizes)

size_ticks = np.arange(1, sizes.shape[0] + 1)

plt.figure()
plt.boxplot(times)
plt.xticks(size_ticks, sizes)
plt.xlabel('Connections')
plt.ylabel('Time (s)')
plt.title('Time to Make Records v. Number Connections')
plt.grid()
plt.savefig(os.path.join('plots', 'ddos_times.png'))

