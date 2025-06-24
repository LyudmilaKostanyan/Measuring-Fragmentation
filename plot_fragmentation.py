import matplotlib
matplotlib.use('Agg')

import matplotlib.pyplot as plt
import pandas as pd
import sys

data = pd.read_csv(sys.argv[1])

fig, ax1 = plt.subplots()

ax1.plot(data['iteration'], data['rss_kb'], 'r-', label='RSS (kB)')
ax1.plot(data['iteration'], data['actual_allocated'] / 1024, 'g--', label='Actual Allocated (kB)')
ax1.plot(data['iteration'], data['mapped'] / 1024, 'orange', label='Mapped (kB)')

ax1.set_xlabel('Iteration')
ax1.set_ylabel('Memory (kB)', color='black')
ax1.tick_params(axis='y', labelcolor='black')
ax1.legend(loc='upper left')

ax2 = ax1.twinx()
ax2.plot(data['iteration'], data['fragmentation_percent'], 'b-', label='Fragmentation (%)')
ax2.set_ylabel('Fragmentation (%)', color='blue')
ax2.tick_params(axis='y', labelcolor='blue')
ax2.legend(loc='upper right')

plt.title('Memory Usage and Fragmentation Over Time')
fig.tight_layout()
plt.savefig("plot.png")
