#ifndef GRAPH_H
#define GRAPH_H

#define MAX_CITIES 20
#define INF        99999

typedef struct {
    char name[50];
} City;

typedef struct {
    int   num_cities;
    City  cities[MAX_CITIES];
    int   dist[MAX_CITIES][MAX_CITIES]; // adjacency matrix
} Graph;

void graph_init(Graph *g);
void graph_add_city(Graph *g, const char *name);
void graph_add_edge(Graph *g, int u, int v, int weight);
void graph_print(Graph *g);
void graph_load_sample(Graph *g);

// Algorithms
void dijkstra(Graph *g, int src);
void bellman_ford(Graph *g, int src);
void floyd_warshall(Graph *g);
void bfs(Graph *g, int src);
void dfs(Graph *g, int src);
void topological_sort(Graph *g);

#endif
