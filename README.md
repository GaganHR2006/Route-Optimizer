# Smart Logistics & Route Optimizer v2.0
### Design and Analysis of Algorithms — Mini Project (C)

A comprehensive visualization engine that demonstrates **core DAA algorithms** across **5 paradigms**, solving real-world logistics and optimization problems with an **enhanced interactive GUI** that brings algorithms to life.

---

## 🎯 Project Objective

**Main Agenda: Implement algorithms and show how they solve real-world problems.**

This project goes beyond textbook implementations to demonstrate:
- How algorithms are applied to practical scenarios (GPS, Network Design, Cargo Loading)
- Why certain algorithms work better for specific problems
- Trade-offs between different algorithmic approaches
- Step-by-step visualization with storytelling explanations

---

## 📊 Algorithms Implemented (Syllabus-Focused)

### Graph Algorithms (Route Planning)
| Algorithm | Paradigm | Complexity | Real-World Use |
|-----------|----------|------------|----------------|
| **Dijkstra** | Greedy | O(V²) | GPS Navigation, Shortest Routes |
| Bellman-Ford | Dynamic Programming | O(V·E) | Currency Arbitrage Detection |
| Floyd-Warshall | Dynamic Programming | O(V³) | Network Analysis |
| BFS | Graph Traversal | O(V+E) | Social Networks, Web Crawling |
| DFS | Graph Traversal | O(V+E) | Maze Solving, Cycle Detection |
| Topological Sort | Decrease & Conquer | O(V+E) | Build Systems, Task Scheduling |

### Minimum Spanning Tree (Network Design)
| Algorithm | Paradigm | Complexity | Real-World Use |
|-----------|----------|------------|----------------|
| Prim's MST | Greedy | O(V²) | Network Infrastructure |
| **Kruskal's MST** | Greedy | O(E log E) | Fiber Optic Network Design |

### Optimization Problems
| Algorithm | Paradigm | Complexity | Real-World Use |
|-----------|----------|------------|----------------|
| **0/1 Knapsack** | Dynamic Programming | O(n·W) | Cargo Loading, Resource Allocation |
| Fractional Knapsack | Greedy | O(n log n) | Continuous Resource Allocation |
| **TSP Branch & Bound** | Backtracking + B&B | O(n!) pruned | Delivery Route Optimization |

---

## 🎓 Algorithm Paradigms Covered

| Paradigm | Description | Algorithms |
|----------|-------------|------------|
| **Greedy** | Make locally optimal choice at each step | Dijkstra, Prim, Kruskal, Fractional Knapsack |
| **Dynamic Programming** | Break into overlapping subproblems, memoize | 0/1 Knapsack, Floyd-Warshall, Bellman-Ford |
| **Divide & Conquer** | Split, solve recursively, combine | Merge Sort, Quick Sort |
| **Branch & Bound** | Systematic enumeration with pruning | TSP |
| **Graph Traversal** | Systematic exploration of graph structure | BFS, DFS, Topological Sort |

---

## 🎨 Enhanced GUI Visualizer Features

The GUI visualizer has been significantly enhanced with:

### Visual Improvements
- **Title bars** with algorithm name and progress indicators
- **Glow effects** for current nodes/edges being processed
- **Color-coded status** (unvisited, current, finalized, rejected)
- **Progress bars** showing algorithm completion
- **Paradigm badges** (GREEDY, DP, BRANCH&BOUND)
- **Complexity indicators** (O(V²), O(E log E), etc.)

### Storytelling & Education
- **Real-world problem context** for each algorithm
- **Step-by-step explanations** in story bar
- **Visual comparison** between approaches (BF vs B&B, DP vs Greedy)
- **Statistics panels** showing nodes explored, pruning efficiency

### Algorithm Visualizations

1. **Dijkstra's Shortest Path**
   - GPS navigation scenario: Find cheapest routes from Bangalore
   - Nodes show distance values, color indicates status
   - Distance table with visual indicators
   - Legend explaining node colors

2. **Kruskal's MST**
   - Network design scenario: Minimize fiber optic cable cost
   - Edges sorted by weight, shown with ADD/CYCLE status
   - MST statistics (edges added, total cost)
   - Connected nodes highlighted in green

3. **TSP Comparison**
   - Delivery route scenario: Visit all cities and return
   - Side-by-side Brute Force vs Branch & Bound
   - Visual bar chart comparing nodes explored
   - Pruning efficiency percentage

4. **0/1 Knapsack**
   - Cargo loading scenario: Maximize truck value
   - Items shown as bars with weight/value/ratio
   - Capacity gauge showing fill level
   - DP optimal vs Greedy fractional comparison

---

## 📁 Project Structure

```
RouteOptimizer/
├── main.c                 # CLI with 7 focused menu options
├── Makefile               # Build configuration
├── include/
│   ├── graph.h            # Graph data structures & shortest path algorithms
│   ├── mst.h              # Minimum Spanning Tree algorithms
│   ├── knapsack.h         # Knapsack problem variants
│   ├── tsp.h              # Traveling Salesman Problem
│   ├── algorithms.h       # Additional algorithm utilities
│   ├── benchmark.h        # Timer & operation counter
│   └── visualizer.h       # Raylib GUI visualizer
├── src/
│   ├── graph.c            # Dijkstra, Bellman-Ford, Floyd-Warshall, BFS, DFS
│   ├── mst.c              # Prim's, Kruskal's with Union-Find DSU
│   ├── knapsack.c         # 0/1 DP + Greedy Fractional
│   ├── tsp.c              # Brute Force + Branch & Bound
│   ├── algorithms.c       # Sorting comparison, utilities
│   ├── benchmark.c        # Performance measurement
│   └── visualizer.c       # Enhanced interactive GUI demonstrations
└── data/
    └── sample_graph.txt   # Sample input data
```

---

## 🚀 Build & Run

### Windows (MinGW/TDM-GCC)

```bash
# Build with GUI visualizer (requires Raylib)
mingw32-make

# Build CLI-only version (no Raylib dependency)
mingw32-make cli

# Clean build artifacts
mingw32-make clean
```

### Compiler Requirements
- GCC with C99 support
- Raylib library (for GUI visualizer)
- Windows: TDM-GCC or MinGW-w64 recommended

---

## 📋 Menu Options

```
╔══════════════════════════════════════════════════════════════╗
║          SMART LOGISTICS & ROUTE OPTIMIZER v2.0              ║
║     Design & Analysis of Algorithms - Mini Project           ║
╠══════════════════════════════════════════════════════════════╣
║  1. Shortest Path Comparison (Dijkstra/Bellman-Ford/Floyd)   ║
║  2. MST Verification (Prim vs Kruskal)                       ║
║  3. Knapsack Trade-off (0/1 DP vs Greedy)                    ║
║  4. TSP Branch & Bound Pruning Demo                          ║
║  5. Graph Traversal (BFS/DFS/Topological Sort)               ║
║  6. Sorting Complexity Comparison                            ║
║  7. Launch GUI Visualizer                                    ║
║  0. Exit                                                     ║
╚══════════════════════════════════════════════════════════════╝
```

---

## 🎮 GUI Controls

| Key | Action |
|-----|--------|
| **1-4** | Select algorithm from menu |
| **ENTER** | Start visualization |
| **SPACE** | Step through algorithm |
| **A** | Toggle auto-advance mode |
| **R** | Reset visualization |
| **ESC** | Go back / Exit |

---

## 📚 Educational Value

### What Students Learn

1. **Algorithm Design Paradigms**
   - When to use Greedy vs Dynamic Programming
   - How Branch & Bound reduces exponential search
   - Trade-offs between optimal and approximate solutions

2. **Complexity Analysis**
   - Visual proof of O(V²), O(E log E), O(n·W) complexity
   - Operation counting during execution
   - Comparison of algorithm efficiency

3. **Real-World Applications**
   - GPS navigation (Dijkstra)
   - Network infrastructure (Kruskal)
   - Cargo optimization (Knapsack)
   - Delivery routing (TSP)

4. **Data Structures**
   - Priority queues (Dijkstra)
   - Union-Find/DSU (Kruskal)
   - DP tables (Knapsack)
   - Recursion trees (TSP)

---

## 📖 Syllabus Coverage

| Topic | Algorithms | Status |
|-------|------------|--------|
| Greedy Method | Dijkstra, Prim, Kruskal, Fractional Knapsack | ✅ |
| Dynamic Programming | 0/1 Knapsack, Floyd-Warshall, Bellman-Ford | ✅ |
| Branch & Bound | TSP | ✅ |
| Graph Traversal | BFS, DFS, Topological Sort | ✅ |
| Divide & Conquer | Merge Sort, Quick Sort | ✅ |

---

## 🏆 What Makes This Project Special

1. **Visual Learning** - See algorithms execute step-by-step with explanations
2. **Real-World Context** - Every algorithm solves a practical problem
3. **Comparison Mode** - Understand trade-offs between approaches
4. **Interactive** - Control the pace of visualization
5. **Educational Stories** - Each step explains what's happening and why

---

## 👨‍💻 Author

DAA Mini Project - Algorithm Visualization & Analysis
