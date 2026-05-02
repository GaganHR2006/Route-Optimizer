#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "mst.h"
#include "knapsack.h"
#include "tsp.h"
#include "benchmark.h"

void print_banner() {
    printf("\n");
    printf("  +================================================+\n");
    printf("  |   SMART LOGISTICS & ROUTE OPTIMIZER  v1.0     |\n");
    printf("  |         DAA Mini Project | C Engine            |\n");
    printf("  +================================================+\n");
}

void print_menu() {
    printf("\n  +-----------------------------------------+\n");
    printf("  |  MODULE SELECT                          |\n");
    printf("  |  1. Route Planner   (Dijkstra / BF / FW)|\n");
    printf("  |  2. Network Infra   (MST: Prim/Kruskal) |\n");
    printf("  |  3. Cargo Optimizer (Knapsack DP/Greedy) |\n");
    printf("  |  4. Delivery Route  (TSP B&B)           |\n");
    printf("  |  5. Load Sample Network                 |\n");
    printf("  |  0. Exit                                |\n");
    printf("  +-----------------------------------------+\n");
    printf("  Choice: ");
}

int main() {
    Graph g;
    graph_init(&g);
    int choice;

    print_banner();
    printf("\n  [INFO] No network loaded. Use option 5 to load sample.\n");

    while (1) {
        print_menu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                if (g.num_cities == 0) { printf("  [!] Load a network first (option 5).\n"); break; }
                graph_print(&g);
                printf("\n  Running all 3 shortest-path algorithms from [0] %s...\n",
                       g.cities[0].name);
                dijkstra(&g, 0);
                bellman_ford(&g, 0);
                floyd_warshall(&g);
                printf("\n  --- Traversals ---\n");
                bfs(&g, 0);
                dfs(&g, 0);
                printf("\n  +==================================================+\n");
                printf("  |  COMPLEXITY COMPARISON                          |\n");
                printf("  |  Dijkstra       O(V^2)  -- single source        |\n");
                printf("  |  Bellman-Ford   O(V*E)  -- handles neg weights  |\n");
                printf("  |  Floyd-Warshall O(V^3)  -- all pairs            |\n");
                printf("  +==================================================+\n");
                break;
            case 2:
                if (g.num_cities == 0) { printf("  [!] Load a network first (option 5).\n"); break; }
                printf("  Running Prim's MST...\n");      prim(&g, 0);
                printf("  Running Kruskal's MST...\n");   kruskal(&g);
                break;
            case 3:
                printf("  Running Knapsack modules...\n");
                knapsack_01(NULL, 0, 0);
                knapsack_fractional(NULL, 0, 0);
                break;
            case 4:
                if (g.num_cities == 0) { printf("  [!] Load a network first (option 5).\n"); break; }
                printf("  Running TSP Branch & Bound...\n");
                tsp_branch_and_bound(&g);
                break;
            case 5:
                graph_load_sample(&g);
                printf("  [OK] Sample Indian logistics network loaded.\n");
                graph_print(&g);
                break;
            case 0:
                printf("\n  Exiting. Goodbye.\n\n");
                return 0;
            default:
                printf("  [!] Invalid choice.\n");
        }
    }
}
