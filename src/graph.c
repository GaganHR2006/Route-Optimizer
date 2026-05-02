#include "graph.h"
#include <stdio.h>
#include <string.h>

void graph_init(Graph *g) {
    g->num_cities = 0;
    for (int i = 0; i < MAX_CITIES; i++)
        for (int j = 0; j < MAX_CITIES; j++)
            g->dist[i][j] = (i == j) ? 0 : INF;
}

void graph_add_city(Graph *g, const char *name) {
    strcpy(g->cities[g->num_cities++].name, name);
}

void graph_add_edge(Graph *g, int u, int v, int weight) {
    g->dist[u][v] = weight;
    g->dist[v][u] = weight; // undirected
}

void graph_print(Graph *g) {
    printf("\n  Cities: ");
    for (int i = 0; i < g->num_cities; i++)
        printf("[%d]%s ", i, g->cities[i].name);
    printf("\n");
}

void graph_load_sample(Graph *g) {
    graph_init(g);
    // 6-city Indian logistics network
    graph_add_city(g, "Bangalore");   // 0
    graph_add_city(g, "Chennai");     // 1
    graph_add_city(g, "Hyderabad");   // 2
    graph_add_city(g, "Mumbai");      // 3
    graph_add_city(g, "Pune");        // 4
    graph_add_city(g, "Delhi");       // 5

    graph_add_edge(g, 0, 1, 346);
    graph_add_edge(g, 0, 2, 575);
    graph_add_edge(g, 1, 2, 627);
    graph_add_edge(g, 2, 3, 711);
    graph_add_edge(g, 3, 4, 149);
    graph_add_edge(g, 4, 5, 1413);
    graph_add_edge(g, 2, 5, 1568);
    graph_add_edge(g, 0, 3, 984);
}

// Stubs — implemented next session
void dijkstra(Graph *g, int src)       { (void)g; (void)src; printf("  [TODO] Dijkstra\n"); }
void bellman_ford(Graph *g, int src)   { (void)g; (void)src; printf("  [TODO] Bellman-Ford\n"); }
void floyd_warshall(Graph *g)          { (void)g;            printf("  [TODO] Floyd-Warshall\n"); }
void bfs(Graph *g, int src)            { (void)g; (void)src; printf("  [TODO] BFS\n"); }
void dfs(Graph *g, int src)            { (void)g; (void)src; printf("  [TODO] DFS\n"); }
