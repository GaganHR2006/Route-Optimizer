#include "graph.h"
#include "benchmark.h"
#include <stdio.h>
#include <string.h>

/* ─────────────────────────────────────────
   GRAPH SETUP
───────────────────────────────────────── */

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
    g->dist[v][u] = weight;
}

void graph_print(Graph *g) {
    printf("\n  Cities loaded:\n");
    for (int i = 0; i < g->num_cities; i++)
        printf("    [%d] %s\n", i, g->cities[i].name);
}

void graph_load_sample(Graph *g) {
    graph_init(g);
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

/* ─────────────────────────────────────────
   HELPER — print path by tracing parent[]
───────────────────────────────────────── */

static void print_path(int parent[], int j, City cities[]) {
    if (parent[j] == -1) {
        printf("%s", cities[j].name);
        return;
    }
    print_path(parent, parent[j], cities);
    printf(" -> %s", cities[j].name);
}

static void print_distances(int dist[], int parent[], int n,
                             City cities[], int src) {
    printf("\n  %-15s %-10s  %s\n", "Destination", "Dist(km)", "Path");
    printf("  %-15s %-10s  %s\n",   "───────────", "────────", "────");
    for (int i = 0; i < n; i++) {
        if (i == src) continue;
        if (dist[i] == INF)
            printf("  %-15s %-10s  UNREACHABLE\n",
                   cities[i].name, "INF");
        else {
            printf("  %-15s %-10d  ", cities[i].name, dist[i]);
            print_path(parent, i, cities);
            printf("\n");
        }
    }
}

/* ─────────────────────────────────────────
   ALGORITHM 1 — DIJKSTRA  O(V²)
───────────────────────────────────────── */

void dijkstra(Graph *g, int src) {
    int   n = g->num_cities;
    int   dist[MAX_CITIES], visited[MAX_CITIES], parent[MAX_CITIES];
    Timer t;

    for (int i = 0; i < n; i++) {
        dist[i]    = INF;
        visited[i] = 0;
        parent[i]  = -1;
    }
    dist[src] = 0;

    timer_start(&t);

    for (int count = 0; count < n - 1; count++) {
        /* pick unvisited vertex with minimum distance */
        int u = -1;
        for (int v = 0; v < n; v++) {
            t.operations++;
            if (!visited[v] && (u == -1 || dist[v] < dist[u]))
                u = v;
        }
        if (u == -1 || dist[u] == INF) break;
        visited[u] = 1;

        /* relax neighbours */
        for (int v = 0; v < n; v++) {
            t.operations++;
            if (!visited[v] &&
                g->dist[u][v] != INF &&
                dist[u] + g->dist[u][v] < dist[v]) {
                dist[v]   = dist[u] + g->dist[u][v];
                parent[v] = u;
            }
        }
    }

    timer_stop(&t);

    printf("\n  +--- DIJKSTRA (Single-Source Shortest Path) ---+\n");
    printf("  |  Source: %s\n", g->cities[src].name);
    printf("  |  Complexity: O(V^2)  |  V = %d cities\n", n);
    print_distances(dist, parent, n, g->cities, src);
    timer_print(&t, "Dijkstra");
}

/* ─────────────────────────────────────────
   ALGORITHM 2 — BELLMAN-FORD  O(V·E)
   Handles negative weights — Dijkstra can't
───────────────────────────────────────── */

void bellman_ford(Graph *g, int src) {
    int   n = g->num_cities;
    int   dist[MAX_CITIES], parent[MAX_CITIES];
    Timer t;

    for (int i = 0; i < n; i++) {
        dist[i]   = INF;
        parent[i] = -1;
    }
    dist[src] = 0;

    timer_start(&t);

    /* relax all edges V-1 times */
    for (int iter = 0; iter < n - 1; iter++) {
        int updated = 0;
        for (int u = 0; u < n; u++) {
            for (int v = 0; v < n; v++) {
                t.operations++;
                if (g->dist[u][v] != INF && dist[u] != INF &&
                    dist[u] + g->dist[u][v] < dist[v]) {
                    dist[v]   = dist[u] + g->dist[u][v];
                    parent[v] = u;
                    updated   = 1;
                }
            }
        }
        if (!updated) break; /* early exit if stable */
    }

    /* negative cycle detection */
    int has_neg_cycle = 0;
    for (int u = 0; u < n; u++) {
        for (int v = 0; v < n; v++) {
            t.operations++;
            if (g->dist[u][v] != INF && dist[u] != INF &&
                dist[u] + g->dist[u][v] < dist[v]) {
                has_neg_cycle = 1;
            }
        }
    }

    timer_stop(&t);

    printf("\n  +--- BELLMAN-FORD (Handles Negative Weights) ---+\n");
    printf("  |  Source: %s\n", g->cities[src].name);
    printf("  |  Complexity: O(V*E)  |  V=%d  E~%d\n", n, n * n);
    if (has_neg_cycle)
        printf("  |  WARNING: NEGATIVE CYCLE DETECTED -- distances invalid\n");
    else
        print_distances(dist, parent, n, g->cities, src);
    timer_print(&t, "Bellman-Ford");
}

/* ─────────────────────────────────────────
   ALGORITHM 3 — FLOYD-WARSHALL  O(V³)
   All-pairs shortest paths in one shot
───────────────────────────────────────── */

void floyd_warshall(Graph *g) {
    int   n = g->num_cities;
    int   d[MAX_CITIES][MAX_CITIES];
    int   next[MAX_CITIES][MAX_CITIES]; /* path reconstruction */
    Timer t;

    /* initialise */
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            d[i][j]    = g->dist[i][j];
            next[i][j] = (g->dist[i][j] != INF && i != j) ? j : -1;
        }
    }

    timer_start(&t);

    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                t.operations++;
                if (d[i][k] != INF && d[k][j] != INF &&
                    d[i][k] + d[k][j] < d[i][j]) {
                    d[i][j]    = d[i][k] + d[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }

    timer_stop(&t);

    printf("\n  +--- FLOYD-WARSHALL (All-Pairs Shortest Paths) ---+\n");
    printf("  |  Complexity: O(V^3)  |  V = %d cities\n", n);
    printf("\n  %-15s", "FROM \\ TO");
    for (int j = 0; j < n; j++)
        printf("  %-10s", g->cities[j].name);
    printf("\n  ");
    for (int j = 0; j <= n; j++) printf("------------");
    printf("\n");

    for (int i = 0; i < n; i++) {
        printf("  %-15s", g->cities[i].name);
        for (int j = 0; j < n; j++) {
            if (d[i][j] == INF)
                printf("  %-10s", "INF");
            else if (i == j)
                printf("  %-10s", "-");
            else
                printf("  %-10d", d[i][j]);
        }
        printf("\n");
    }
    timer_print(&t, "Floyd-Warshall");
}

/* ─────────────────────────────────────────
   BFS / DFS — Unit 2 coverage
───────────────────────────────────────── */

static int bfs_visited[MAX_CITIES];
static int dfs_visited[MAX_CITIES];

void bfs(Graph *g, int src) {
    int n = g->num_cities;
    int queue[MAX_CITIES], front = 0, rear = 0;
    Timer t;

    for (int i = 0; i < n; i++) bfs_visited[i] = 0;
    bfs_visited[src] = 1;
    queue[rear++] = src;

    printf("\n  BFS traversal from %s:\n  ", g->cities[src].name);

    timer_start(&t);
    while (front < rear) {
        int u = queue[front++];
        printf("%s ", g->cities[u].name);
        for (int v = 0; v < n; v++) {
            t.operations++;
            if (g->dist[u][v] != INF && g->dist[u][v] != 0
                && !bfs_visited[v]) {
                bfs_visited[v] = 1;
                queue[rear++]  = v;
            }
        }
    }
    timer_stop(&t);
    printf("\n");
    timer_print(&t, "BFS");
}

static void dfs_visit(Graph *g, int u, Timer *t) {
    dfs_visited[u] = 1;
    printf("%s ", g->cities[u].name);
    for (int v = 0; v < g->num_cities; v++) {
        t->operations++;
        if (g->dist[u][v] != INF && g->dist[u][v] != 0
            && !dfs_visited[v])
            dfs_visit(g, v, t);
    }
}

void dfs(Graph *g, int src) {
    Timer t;
    for (int i = 0; i < g->num_cities; i++) dfs_visited[i] = 0;

    printf("\n  DFS traversal from %s:\n  ", g->cities[src].name);
    timer_start(&t);
    dfs_visit(g, src, &t);
    timer_stop(&t);
    printf("\n");
    timer_print(&t, "DFS");
}

/* -----------------------------------------
   TOPOLOGICAL SORT  O(V + E)
   Unit 2 -- Decrease and Conquer.
   Only valid for Directed Acyclic Graphs (DAG).
   Use case: course prerequisite scheduling.
----------------------------------------- */

static int  topo_stack[MAX_CITIES];
static int  topo_top;
static int  topo_visited[MAX_CITIES];

static void topo_dfs(Graph *g, int u) {
    topo_visited[u] = 1;
    for (int v = 0; v < g->num_cities; v++) {
        if (g->dist[u][v] != INF && g->dist[u][v] != 0
            && !topo_visited[v])
            topo_dfs(g, v);
    }
    topo_stack[topo_top++] = u; /* push after all descendants */
}

void topological_sort(Graph *g) {
    Timer t;

    /* Build a small DAG for course prerequisites */
    Graph dag;
    graph_init(&dag);
    graph_add_city(&dag, "Algorithms");       /* 0 */
    graph_add_city(&dag, "Data Structures");  /* 1 */
    graph_add_city(&dag, "Graph Theory");     /* 2 */
    graph_add_city(&dag, "Dynamic Prog.");    /* 3 */
    graph_add_city(&dag, "NP-Completeness"); /* 4 */
    graph_add_city(&dag, "Advanced DAA");    /* 5 */

    /* directed edges: prerequisite -> course */
    dag.dist[1][0] = 1;   /* Data Structures -> Algorithms   */
    dag.dist[0][2] = 1;   /* Algorithms      -> Graph Theory */
    dag.dist[0][3] = 1;   /* Algorithms      -> Dynamic Prog */
    dag.dist[2][4] = 1;   /* Graph Theory    -> NP-Complete  */
    dag.dist[3][4] = 1;   /* Dynamic Prog    -> NP-Complete  */
    dag.dist[4][5] = 1;   /* NP-Complete     -> Advanced DAA */

    int dn = dag.num_cities;

    for (int i = 0; i < dn; i++) topo_visited[i] = 0;
    topo_top = 0;

    timer_start(&t);
    for (int i = 0; i < dn; i++)
        if (!topo_visited[i])
            topo_dfs(&dag, i);
    timer_stop(&t);

    printf("\n  +--- TOPOLOGICAL SORT  O(V+E) ---+\n");
    printf("  |  Technique   : Decrease-and-Conquer (DFS-based)\n");
    printf("  |  Use case    : Course prerequisite scheduling\n");
    printf("  |  Complexity  : O(V+E)  V=%d  E=6\n\n", dn);

    printf("  Dependency graph:\n");
    printf("  Data Structures -> Algorithms -> Graph Theory -+\n");
    printf("                              +-> Dynamic Prog.  +--> NP-Complete -> Advanced DAA\n");
    printf("                                                 +\n");

    printf("\n  Valid course order (Topological sequence):\n  ");
    for (int i = topo_top - 1; i >= 0; i--) {
        printf("%s", dag.cities[topo_stack[i]].name);
        if (i > 0) printf("  ->  ");
    }
    printf("\n");
    timer_print(&t, "Topological Sort");

    (void)g; /* g passed for API consistency — DAG built internally */
}
