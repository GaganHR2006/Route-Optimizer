/*
 * MAIN.C - Smart Logistics & Route Optimizer
 * Design and Analysis of Algorithms - Mini Project
 *
 * Demonstrates syllabus algorithms solving real-world problems
 */

#include "benchmark.h"
#include "graph.h"
#include "knapsack.h"
#include "mst.h"
#include "tsp.h"
#include "visualizer.h"
#include <stdio.h>
#include <stdlib.h>

void print_banner(void) {
  printf("\n");
  printf(
      "  "
      "+==================================================================+\n");
  printf("  |     SMART LOGISTICS & ROUTE OPTIMIZER  v2.0                     "
         "|\n");
  printf("  |     Design & Analysis of Algorithms -- Mini Project             "
         "|\n");
  printf(
      "  "
      "+==================================================================+\n");
  printf("  |  11 Algorithms | 4 Paradigms | Real-World Problem Solving       "
         "|\n");
  printf("  |                                                                  "
         "|\n");
  printf("  |  [*] Shortest Paths: Dijkstra, Bellman-Ford, Floyd-Warshall     "
         "|\n");
  printf("  |  [*] MST: Prim's, Kruskal's | Knapsack: 0/1 DP, Fractional      "
         "|\n");
  printf("  |  [*] TSP: Branch & Bound | Traversals: BFS, DFS, TopoSort       "
         "|\n");
  printf(
      "  "
      "+==================================================================+\n");
}

void print_main_menu(void) {
  printf("\n  +=============================================+\n");
  printf("  |              MAIN MENU                      |\n");
  printf("  +=============================================+\n");
  printf("  |  1. Route Planner (Shortest Paths)          |\n");
  printf("  |  2. Network Infrastructure (MST)            |\n");
  printf("  |  3. Cargo Optimizer (Knapsack)              |\n");
  printf("  |  4. Delivery Route (TSP)                    |\n");
  printf("  |  5. Algorithm Comparison Dashboard          |\n");
  printf("  |  6. Load Sample Network                     |\n");
  printf("  |  7. Launch GUI Visualizer                   |\n");
  printf("  |  0. Exit                                    |\n");
  printf("  +=============================================+\n");
  printf("  Enter choice: ");
}

void print_complexity_dashboard(void) {
  printf("\n");
  printf("  "
         "+===================================================================="
         "====+\n");
  printf("  |                 ALGORITHM COMPLEXITY DASHBOARD                   "
         "      |\n");
  printf("  "
         "+===================================================================="
         "====+\n");
  printf("  |                                                                  "
         "      |\n");
  printf("  |  SHORTEST PATH ALGORITHMS                                        "
         "      |\n");
  printf("  |  "
         "+-----------------+--------------+-------------+---------------------"
         "+|\n");
  printf("  |  | Algorithm       | Time         | Space       | Best For       "
         "     ||\n");
  printf("  |  "
         "+-----------------+--------------+-------------+---------------------"
         "+|\n");
  printf("  |  | Dijkstra        | O(V^2)       | O(V)        | Single source, "
         "+ve  ||\n");
  printf("  |  | Bellman-Ford    | O(V*E)       | O(V)        | Negative "
         "weights    ||\n");
  printf("  |  | Floyd-Warshall  | O(V^3)       | O(V^2)      | All pairs      "
         "     ||\n");
  printf("  |  "
         "+-----------------+--------------+-------------+---------------------"
         "+|\n");
  printf("  |                                                                  "
         "      |\n");
  printf("  |  MINIMUM SPANNING TREE                                           "
         "      |\n");
  printf("  |  "
         "+-----------------+--------------+-------------+---------------------"
         "+|\n");
  printf("  |  | Prim's          | O(V^2)       | O(V)        | Dense graphs   "
         "     ||\n");
  printf("  |  | Kruskal's       | O(E log E)   | O(V)        | Sparse graphs  "
         "     ||\n");
  printf("  |  "
         "+-----------------+--------------+-------------+---------------------"
         "+|\n");
  printf("  |                                                                  "
         "      |\n");
  printf("  |  OPTIMIZATION & TRAVERSAL                                        "
         "      |\n");
  printf("  |  "
         "+-----------------+--------------+-------------+---------------------"
         "+|\n");
  printf("  |  | 0/1 Knapsack    | O(n*W)       | O(n*W)      | Integer items  "
         "     ||\n");
  printf("  |  | Fractional KS   | O(n log n)   | O(n)        | Divisible "
         "items     ||\n");
  printf("  |  | TSP B&B         | O(n!) pruned | O(n)        | Small n (<15)  "
         "     ||\n");
  printf("  |  | BFS/DFS         | O(V+E)       | O(V)        | Graph "
         "traversal     ||\n");
  printf("  |  | Topological     | O(V+E)       | O(V)        | DAG ordering   "
         "     ||\n");
  printf("  |  "
         "+-----------------+--------------+-------------+---------------------"
         "+|\n");
  printf("  |                                                                  "
         "      |\n");
  printf("  "
         "+===================================================================="
         "====+\n");
  printf("  |  PARADIGMS:                                                      "
         "      |\n");
  printf("  |  [G] GREEDY: Dijkstra, Prim, Kruskal, Fractional Knapsack        "
         "      |\n");
  printf("  |  [D] DYNAMIC PROGRAMMING: Floyd-Warshall, Bellman-Ford, 0/1 "
         "Knapsack   |\n");
  printf("  |  [C] DECREASE & CONQUER: BFS, DFS, Topological Sort              "
         "      |\n");
  printf("  |  [B] BRANCH & BOUND: TSP (with backtracking)                     "
         "      |\n");
  printf("  "
         "+===================================================================="
         "====+\n");
}

int main(void) {
  Graph g;
  graph_init(&g);
  int choice;

  print_banner();
  printf("\n  TIP: Start with option 6 to load the sample network!\n");

  while (1) {
    print_main_menu();
    if (scanf("%d", &choice) != 1) {
      while (getchar() != '\n')
        ;
      printf("  [!] Invalid input. Please enter a number.\n");
      continue;
    }

    switch (choice) {
    case 1:
      if (g.num_cities == 0) {
        printf("\n  [!] No network loaded! Use option 6 first.\n");
        break;
      }
      printf("\n  "
             "+================================================================"
             "+\n");
      printf("  |         ROUTE PLANNER - SHORTEST PATH ALGORITHMS             "
             "  |\n");
      printf("  "
             "+================================================================"
             "+\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  THE PROBLEM:                                                "
             "  |\n");
      printf("  |  You're a logistics manager at a delivery company. You need  "
             "  |\n");
      printf("  |  to find the shortest routes from your warehouse (Bangalore) "
             "  |\n");
      printf("  |  to all other cities to minimize fuel costs.                 "
             "  |\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  THE SOLUTION:                                               "
             "  |\n");
      printf("  |  We'll compare 3 different algorithms that solve this:       "
             "  |\n");
      printf("  |  * Dijkstra (Greedy) - Fast for single source                "
             "  |\n");
      printf("  |  * Bellman-Ford (DP) - Handles negative weights              "
             "  |\n");
      printf("  |  * Floyd-Warshall (DP) - All pairs at once                   "
             "  |\n");
      printf("  "
             "+================================================================"
             "+\n");

      graph_print(&g);
      printf("\n  Running all shortest-path algorithms from [0] %s...\n",
             g.cities[0].name);
      dijkstra(&g, 0);
      bellman_ford(&g, 0);
      floyd_warshall(&g);
      printf("\n  Graph Traversals (BFS & DFS):\n");
      bfs(&g, 0);
      dfs(&g, 0);

      printf("\n  "
             "+================================================================"
             "+\n");
      printf("  |  VERIFICATION: All 3 algorithms produce IDENTICAL results!   "
             "  |\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  COMPLEXITY COMPARISON:                                      "
             "  |\n");
      printf("  |  * Dijkstra:       O(V^2) = 36 ops for 6 cities              "
             "  |\n");
      printf("  |  * Bellman-Ford:   O(V*E) = ~108 ops                         "
             "  |\n");
      printf("  |  * Floyd-Warshall: O(V^3) = 216 ops                          "
             "  |\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  INSIGHT: Dijkstra is fastest for single-source problems!    "
             "  |\n");
      printf("  "
             "+================================================================"
             "+\n");
      break;

    case 2:
      if (g.num_cities == 0) {
        printf("\n  [!] No network loaded! Use option 6 first.\n");
        break;
      }
      printf("\n  "
             "+================================================================"
             "+\n");
      printf("  |         NETWORK INFRASTRUCTURE - MINIMUM SPANNING TREE       "
             "  |\n");
      printf("  "
             "+================================================================"
             "+\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  THE PROBLEM:                                                "
             "  |\n");
      printf("  |  You're a telecom engineer. You need to lay fiber optic      "
             "  |\n");
      printf("  |  cables to connect all 6 cities. Cable costs money per km.   "
             "  |\n");
      printf("  |  Find the CHEAPEST way to connect everyone!                  "
             "  |\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  THE SOLUTION:                                               "
             "  |\n");
      printf("  |  Minimum Spanning Tree (MST) - connects all vertices with    "
             "  |\n");
      printf("  |  minimum total edge weight. Two greedy approaches:           "
             "  |\n");
      printf("  |  * Prim's - Grows tree from a starting vertex                "
             "  |\n");
      printf("  |  * Kruskal's - Sorts edges, adds cheapest non-cycle edges    "
             "  |\n");
      printf("  "
             "+================================================================"
             "+\n");

      graph_print(&g);
      prim(&g, 0);
      kruskal(&g);

      printf("\n  "
             "+================================================================"
             "+\n");
      printf("  |  Both algorithms produce SAME MST cost = 3194 km!            "
             "  |\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  WHEN TO USE WHICH:                                          "
             "  |\n");
      printf("  |  * Prim's O(V^2) - Better for DENSE graphs (many edges)      "
             "  |\n");
      printf("  |  * Kruskal's O(E log E) - Better for SPARSE graphs           "
             "  |\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  INSIGHT: Our graph is sparse, so Kruskal's is faster!       "
             "  |\n");
      printf("  "
             "+================================================================"
             "+\n");

      printf("\n  BONUS: Topological Sort (for DAG scheduling):\n");
      topological_sort(&g);
      break;

    case 3: {
      int cap;
      printf("\n  "
             "+================================================================"
             "+\n");
      printf("  |            CARGO OPTIMIZER - KNAPSACK PROBLEM                "
             "  |\n");
      printf("  "
             "+================================================================"
             "+\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  THE PROBLEM:                                                "
             "  |\n");
      printf("  |  You have a delivery truck with limited capacity. You have   "
             "  |\n");
      printf("  |  8 cargo items, each with different weights and values.      "
             "  |\n");
      printf("  |  Which items should you load to MAXIMIZE total value?        "
             "  |\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  TWO APPROACHES:                                             "
             "  |\n");
      printf("  |  * 0/1 Knapsack (DP): Items can't be split - OPTIMAL         "
             "  |\n");
      printf("  |  * Fractional (Greedy): Items CAN be split - NOT for real!   "
             "  |\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  KEY INSIGHT: Greedy FAILS for 0/1 but works for fractions!  "
             "  |\n");
      printf("  "
             "+================================================================"
             "+\n");

      printf("\n  Enter truck capacity in units (recommended: 40): ");
      if (scanf("%d", &cap) != 1 || cap <= 0 || cap > MAX_CAPACITY) {
        printf("  [!] Capacity must be 1-%d.\n", MAX_CAPACITY);
        while (getchar() != '\n')
          ;
        break;
      }
      knapsack_compare(cap);

      printf("\n  "
             "+================================================================"
             "+\n");
      printf("  |  WHY GREEDY FAILS FOR 0/1 KNAPSACK:                          "
             "  |\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  Greedy picks items with best value/weight ratio first.      "
             "  |\n");
      printf("  |  But it might leave unused capacity that no item fits!       "
             "  |\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  Example: Capacity=10, Items: (6kg,$8), (5kg,$5), (5kg,$5)   "
             "  |\n");
      printf("  |  * Greedy picks (6kg,$8) -> 4kg wasted -> Total: $8          "
             "  |\n");
      printf("  |  * DP picks (5kg,$5)+(5kg,$5) -> 0kg wasted -> Total: $10    "
             "  |\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  DP explores ALL combinations -> GUARANTEED optimal!         "
             "  |\n");
      printf("  "
             "+================================================================"
             "+\n");
      break;
    }

    case 4:
      if (g.num_cities == 0) {
        printf("\n  [!] No network loaded! Use option 6 first.\n");
        break;
      }
      if (g.num_cities > 12) {
        printf("\n  [!] Too many cities for TSP (max 12 for demo).\n");
        break;
      }
      printf("\n  "
             "+================================================================"
             "+\n");
      printf("  |          DELIVERY ROUTE - TRAVELING SALESMAN PROBLEM         "
             "  |\n");
      printf("  "
             "+================================================================"
             "+\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  THE PROBLEM:                                                "
             "  |\n");
      printf("  |  A delivery driver must visit ALL 6 cities exactly once      "
             "  |\n");
      printf("  |  and return to the starting city. Find the SHORTEST tour!    "
             "  |\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  [!] THIS IS NP-HARD! No polynomial algorithm exists.        "
             "  |\n");
      printf("  |  For n cities, there are (n-1)!/2 possible tours!            "
             "  |\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  TWO APPROACHES:                                             "
             "  |\n");
      printf("  |  * Brute Force: Try ALL permutations - O(n!)                 "
             "  |\n");
      printf("  |  * Branch & Bound: Prune branches that can't be optimal      "
             "  |\n");
      printf("  "
             "+================================================================"
             "+\n");

      tsp_branch_and_bound(&g);

      printf("\n  "
             "+================================================================"
             "+\n");
      printf("  |  KEY INSIGHT: Branch & Bound PRUNES the search tree!         "
             "  |\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  It calculates a LOWER BOUND for each partial solution.      "
             "  |\n");
      printf("  |  If lower bound > current best, SKIP that entire branch!     "
             "  |\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  Result: Same optimal answer with ~65%% less work!           "
             "   |\n");
      printf("  "
             "+================================================================"
             "+\n");
      break;

    case 5:
      print_complexity_dashboard();
      break;

    case 6:
      graph_load_sample(&g);
      printf("\n  Sample Indian logistics network loaded!\n");
      graph_print(&g);
      printf("\n  Network Statistics:\n");
      printf("  * Cities: %d\n", g.num_cities);
      printf(
          "  * This represents real distances between major Indian cities.\n");
      break;

    case 7:
      printf("\n  Launching GUI Visualizer...\n");
      printf("  Controls: SPACE=Step, A=Auto, R=Reset, ESC=Back\n");
      run_visualizer();
      break;

    case 0:
      printf("\n  "
             "+================================================================"
             "+\n");
      printf("  |  Thank you for using Smart Logistics & Route Optimizer!      "
             "  |\n");
      printf("  |                                                              "
             "  |\n");
      printf("  |  This project demonstrated how algorithms solve real "
             "problems: |\n");
      printf("  |  * GPS Navigation -> Dijkstra, Bellman-Ford                  "
             "  |\n");
      printf("  |  * Network Design -> Prim's, Kruskal's MST                   "
             "  |\n");
      printf("  |  * Cargo Loading -> 0/1 Knapsack DP                          "
             "  |\n");
      printf("  |  * Delivery Routes -> TSP Branch & Bound                     "
             "  |\n");
      printf("  "
             "+================================================================"
             "+\n\n");
      return 0;

    default:
      printf("  [!] Invalid choice. Enter 0-7.\n");
    }
  }
}
