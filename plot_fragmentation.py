import matplotlib.pyplot as plt
import pandas as pd
import sys

data = pd.read_csv(sys.argv[1])

fig, ax1 = plt.subplots()

ax1.plot(data['iteration'], data['memory_usage_kb'], 'r-', label='Memory Usage (kB)')
ax1.set_xlabel('Iteration')
ax1.set_ylabel('Memory Usage (kB)', color='r')
ax1.tick_params(axis='y', colors='red')

ax2 = ax1.twinx()
ax2.plot(data['iteration'], data['allocated_objects'], 'b-', label='Allocated Objects')
ax2.set_ylabel('Allocated Objects', color='b')
ax2.tick_params(axis='y', colors='blue')

fig.tight_layout()
plt.title('Memory Fragmentation Over Time')

plt.savefig("plot.png")
plt.show()
