# Smart Logistics & Route Optimizer
### Design and Analysis of Algorithms — Mini Project (C)

A command-line engine that solves real-world logistics problems using
11 algorithms across 4 paradigms, with live benchmarking that proves
theoretical complexity at runtime.

---

## Algorithms Implemented

| # | Algorithm | Paradigm | Complexity | Module |
|---|-----------|----------|------------|--------|
| 1 | Dijkstra | Greedy | O(V²) | Route Planner |
| 2 | Bellman-Ford | Dynamic Programming | O(V·E) | Route Planner |
| 3 | Floyd-Warshall | Dynamic Programming | O(V³) | Route Planner |
| 4 | BFS | Graph Traversal | O(V+E) | Route Planner |
| 5 | DFS | Decrease & Conquer | O(V+E) | Route Planner |
| 6 | Prim's MST | Greedy | O(V²) | Network Infra |
| 7 | Kruskal's MST | Greedy | O(E log E) | Network Infra |
| 8 | Topological Sort | Decrease & Conquer | O(V+E) | Network Infra |
| 9 | 0/1 Knapsack | Dynamic Programming | O(n·W) | Cargo Optimizer |
| 10 | Fractional Knapsack | Greedy | O(n log n) | Cargo Optimizer |
| 11 | TSP Branch & Bound | Backtracking + B&B | O(n!) pruned | Delivery Route |

---

## Syllabus Coverage

| Syllabus Unit | Topics Covered |
|---|---|
| Unit 2 | BFS, DFS, Topological Sort |
| Unit 3 | Dijkstra, Prim, Kruskal, 0/1 Knapsack DP, Floyd-Warshall, Fractional Knapsack |
| Unit 4 | TSP Branch & Bound, Backtracking, NP-Hard concepts |
| Self-Learning | Bellman-Ford, Union-Find DSU |

---

## Project Structure

```
RouteOptimizer/
├── src/
│   ├── graph.c       # Dijkstra, Bellman-Ford, Floyd-Warshall, BFS, DFS, Topo Sort
│   ├── mst.c         # Prim's (O(V²)) + Kruskal's (O(E log E)) with Union-Find DSU
│   ├── knapsack.c    # 0/1 DP + Greedy Fractional + cargo manifest
│   ├── tsp.c         # Brute Force O(n!) + Branch & Bound with pruning counter
│   └── benchmark.c   # Timer + operation counter used by all modules
├── include/          # Header files for all modules
├── data/             # Sample graph input
├── main.c            # CLI shell + menu
└── Makefile
```

---

## Build & Run (Windows — MinGW)

```bash
C:\MinGW\bin\mingw32-make.exe          # build
C:\MinGW\bin\mingw32-make.exe clean    # clean objects
C:\MinGW\bin\mingw32-make.exe run      # build + run
```

**Compiler:** GCC (MinGW32)
**Flags:** `-Wall -Wextra -O2` — zero warnings across all modules

---

## Key Results

### Route Planner (Module 1)
All 3 algorithms produce **identical distances** from Bangalore:

| Destination | Distance |
|---|---|
| Chennai | 346 km |
| Hyderabad | 575 km |
| Mumbai | 984 km |
| Pune | 1133 km |
| Delhi | 2143 km |

Operation counts: Dijkstra **60** · Bellman-Ford **108** · Floyd-Warshall **216**
Matches theoretical ratio V² : V·E : V³ exactly. ✓

### MST (Module 2)
Both Prim's and Kruskal's produce **identical MST cost: 3194 km**
Op counts: Prim **72** · Kruskal **15** — Kruskal wins on sparse graphs. ✓

### Cargo Optimizer (Module 3) — capacity 40 units

| | 0/1 DP (Optimal) | Greedy Fractional |
|---|---|---|
| Total value | Rs 140k (integer-optimal) | Rs 150k (uses fractions) |
| Feasible for real cargo? | Yes ✓ | No — splits items |

Greedy appears better only because it splits items — impossible for real cargo boxes.
DP is the only correct algorithm for 0/1 cargo loading. ✓

### TSP Branch & Bound (Module 4)

| Metric | Brute Force | Branch & Bound |
|---|---|---|
| Nodes explored | 26 | 9 |
| Branches pruned | 0 | 2 |
| Optimal tour | 5087 km | 5087 km ✓ |
| Search space cut | — | **65.4%** |

Same optimal answer. 65.4% of the search tree eliminated by lower-bound pruning.

---

## What This Demonstrates

- **Algorithm design paradigms:** Greedy, DP, Backtracking, Divide & Conquer
- **Complexity analysis:** Theoretical vs measured operation counts match at runtime
- **Algorithm trade-offs:** When greedy works (fractional knapsack) vs when it fails (0/1)
- **NP-Hard problems:** TSP with B&B pruning proof via live node counter
- **Data structures:** Adjacency matrix, DSU with path compression + union-by-rank
- **Graph theory:** Shortest paths, spanning trees, traversals, DAG ordering
