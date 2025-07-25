# Memory Fragmentation Simulator

## Overview

This project simulates dynamic memory fragmentation over time by repeatedly allocating and freeing randomly sized memory blocks. It tracks and logs memory statistics including:

* Actual memory requested by the program
* Total memory mapped by the allocator (`jemalloc`)
* Physical memory usage (RSS)
* Calculated fragmentation percentage
* Number of live allocations

The results are stored in a CSV file and visualized using Python.

---

## Problem Description

In long-running applications (like servers), memory can become fragmented due to frequent allocation and deallocation of differently sized blocks. This leads to:

* Inefficient memory usage
* Increased memory overhead
* Reduced system performance

This simulation demonstrates how fragmentation arises and evolves under randomized allocation behavior.

---

## Fragmentation Calculation

Fragmentation is computed as:

```text
fragmentation_percent = (mapped - actual_allocated) / mapped * 100
```

Where:

* **`mapped`** is the total memory jemalloc requested from the OS.
* **`actual_allocated`** is the sum of all block sizes currently in use.

This gives an estimate of how much memory is unavailable for reuse due to fragmentation.

> Note: This is an estimate of **external fragmentation**, not including internal allocator overhead.

---

## Example Output

```
Iteration 9000 | Actual Allocated: 437 KB | Mapped: 6436 KB | RSS: 13268 KB | Fragmentation: 93.2084 % | Live allocations: 25
```

---

## Example Plot

Below is a sample graph generated by `plot_fragmentation.py` using the simulation log:

![plot](https://github.com/user-attachments/assets/ca3532ad-708d-48f0-b99f-d83d1592ead1)

### Explanation:

* **Red (RSS)**: Physical memory used by the process (OS-reported).
* **Green dashed (Actual Allocated)**: Sum of requested allocation sizes.
* **Orange (Mapped)**: Memory reserved by jemalloc from the OS.
* **Blue (Fragmentation %)**: Percent of `mapped` memory not directly in use.

You can observe how:

* Allocated memory fluctuates with randomized behavior.
* Fragmentation can remain high even when RSS and mapped memory are stable.
* jemalloc tends to retain memory after freeing it to optimize future allocations.

---

## How to Compile and Run

### 1. Clone the Repository

```bash
git clone https://github.com/LyudmilaKostanyan/MemoryFragmentation.git
cd MemoryFragmentation
```

### 2. Build the Project

```bash
cmake -S . -B build
cmake --build build
```

Make sure you have `jemalloc` installed:

* **Linux**: `sudo apt install libjemalloc-dev`
* **macOS**: `brew install jemalloc`

### 3. Run the Simulation

```bash
cd build
./fragmentation
```

This will create `fragmentation_log.csv`.

---

## Plotting the Results

Generate the plot using:

```bash
python3 plot_fragmentation.py fragmentation_log.csv
```

This creates `plot.png` as shown above.
