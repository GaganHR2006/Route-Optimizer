# Smart Logistics & Route Optimizer

A C-based terminal application implementing core DAA algorithms for logistics and route optimization.

## Project Structure

```
RouteOptimizer/
├── src/
│   ├── graph.c        — Graph data structure + algorithm stubs
│   ├── mst.c          — Prim & Kruskal (MST)
│   ├── knapsack.c     — 0/1 and Fractional Knapsack
│   ├── tsp.c          — TSP Branch & Bound
│   └── benchmark.c    — High-resolution timer module
├── include/
│   ├── graph.h
│   ├── mst.h
│   ├── knapsack.h
│   ├── tsp.h
│   └── benchmark.h
├── data/
│   └── sample_graph.txt   — 6-city Indian logistics network
├── main.c             — CLI shell & module dispatcher
├── Makefile           — Windows/MinGW compatible
└── README.md
```

## Build & Run

**Prerequisites:** MinGW (GCC 6.3.0) — uses `C:\MinGW\bin\gcc.exe`

```bash
# Build (using mingw32-make from C:\MinGW\bin\)
C:\MinGW\bin\mingw32-make.exe          # compile all
C:\MinGW\bin\mingw32-make.exe run      # build + run
C:\MinGW\bin\mingw32-make.exe clean    # remove .o and .exe

# OR compile directly in one shot:
C:\MinGW\bin\gcc.exe -Wall -Wextra -O2 -Iinclude main.c src/graph.c src/mst.c src/knapsack.c src/tsp.c src/benchmark.c -o route_optimizer.exe
.\route_optimizer.exe
```

> **Tip:** Add `C:\MinGW\bin` to your Windows PATH so you can just use `mingw32-make` and `gcc` directly.

## Modules

| Module | Algorithms | Status |
|--------|-----------|--------|
| Route Planner | Dijkstra, Bellman-Ford, Floyd-Warshall | Session 2 |
| Network Infra | Prim's MST, Kruskal's MST | Session 3 |
| Cargo Optimizer | 0/1 Knapsack (DP), Fractional Knapsack (Greedy) | Session 4 |
| Delivery Route | TSP Branch & Bound | Session 5 |
| Benchmark | Timer + op-counter for all algorithms | ✅ Done |

## Sample Network

6-city Indian logistics network (Bangalore, Chennai, Hyderabad, Mumbai, Pune, Delhi) with road distances in km.

## Session Log

- **Session 1** — Scaffold, Makefile, CLI shell, benchmark module, all stubs ✅
