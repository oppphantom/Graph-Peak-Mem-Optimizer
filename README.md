# Graph Peak Memory Optimizer

A high-performance scheduler for minimizing peak memory usage in computational graphs (DAGs). This tool implements both **Exact (Backtracking)** and **Heuristic (Greedy)** algorithms to find the optimal topological execution order.

It is designed to solve memory bottlenecks in embedded AI inference or compiler backend optimization tasks.

##  Key Features

*   **Dual Strategies:**
    *   `Exact Solver`: Uses backtracking with pruning to find the global optimal solution (for small graphs).
    *   `Greedy Solver`: Uses a Min-Heap based heuristic approach for large-scale graphs (10k+ nodes).
*   **Performance Optimization:**
    *   Implemented **Reverse Adjacency List** to achieve $O(1)$ predecessor lookup.
    *   Reduced complexity from $O(VE)$ to $O(E + V \log V)$.
*   **Visualization:** Python scripts included for analyzing memory memory curves.

##  Build & Run

### Prerequisites
*   GCC Compiler
*   Python 3.x (Optional, for visualization)

### Compilation
```bash
mkdir -p bin
gcc src/main.c src/graph.c src/solver_exact.c src/solver_greedy.c -o bin/optimizer

## Usage
```bash
# Run Exact Solver (Default)
./bin/optimizer data/example.txt exact

# Run Greedy Solver (For large graphs)
./bin/optimizer data/example.txt greedy
```
## 📊 Benchmark

| Graph Size | Method | Peak Memory | Time Taken |
| :--- | :--- | :--- | :--- |
| 20 Nodes | Exact | 120 MB | 0.05s |
| 20 Nodes | Greedy | 125 MB (+4.1%) | 0.001s |
| 10,000 Nodes | Greedy | 4050 MB | 0.8s |

## Project Structure
```
├── include/        # Header files
├── src/            # Source code (C)
├── data/           # Test cases
├── plot_memory.py  # Visualization script
└── README.md
```

## License
MIT License. 