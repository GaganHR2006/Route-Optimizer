#include "mst.h"
#include "benchmark.h"
#include <stdio.h>

/* -----------------------------------------
   HELPER -- print the MST edge list
----------------------------------------- */

static void print_mst_edges(int u[], int v[], int w[], int count,
                             City cities[], int total_cost) {
    printf("\n  %-15s  %-15s  %s\n", "From", "To", "Distance(km)");
    printf("  %-15s  %-15s  %s\n",   "----", "--", "------------");
    for (int i = 0; i < count; i++)
        printf("  %-15s  %-15s  %d\n",
               cities[u[i]].name, cities[v[i]].name, w[i]);
    printf("  %s\n", "-------------------------------------");
    printf("  Total MST cost: %d km\n", total_cost);
}

/* -----------------------------------------
   ALGORITHM 1 -- PRIM'S  O(V^2)
   Grows MST one vertex at a time.
   Best for dense graphs.
----------------------------------------- */

void prim(Graph *g, int src) {
    int n = g->num_cities;

    int  in_mst[MAX_CITIES];   /* vertex included in MST?     */
    int  key[MAX_CITIES];      /* min edge weight to reach v  */
    int  parent[MAX_CITIES];   /* MST parent of each vertex   */
    Timer t;

    for (int i = 0; i < n; i++) {
        in_mst[i]  = 0;
        key[i]     = INF;
        parent[i]  = -1;
    }
    key[src] = 0;

    timer_start(&t);

    for (int count = 0; count < n; count++) {
        /* pick vertex not yet in MST with minimum key */
        int u = -1;
        for (int v = 0; v < n; v++) {
            t.operations++;
            if (!in_mst[v] && (u == -1 || key[v] < key[u]))
                u = v;
        }

        in_mst[u] = 1;

        /* update keys of adjacent vertices */
        for (int v = 0; v < n; v++) {
            t.operations++;
            if (!in_mst[v] &&
                g->dist[u][v] != INF &&
                g->dist[u][v] < key[v]) {
                key[v]    = g->dist[u][v];
                parent[v] = u;
            }
        }
    }

    timer_stop(&t);

    /* collect edges for display */
    int eu[MAX_CITIES], ev[MAX_CITIES], ew[MAX_CITIES];
    int edge_count = 0, total = 0;
    for (int v = 0; v < n; v++) {
        if (parent[v] != -1) {
            eu[edge_count] = parent[v];
            ev[edge_count] = v;
            ew[edge_count] = g->dist[parent[v]][v];
            total += ew[edge_count];
            edge_count++;
        }
    }

    printf("\n  +--- PRIM'S MST (Greedy -- Dense Graph Optimal) ---+\n");
    printf("  |  Start vertex : %s\n", g->cities[src].name);
    printf("  |  Complexity   : O(V^2)  |  V = %d cities\n", n);
    printf("  |  Purpose      : Min-cost network to connect all cities\n");
    print_mst_edges(eu, ev, ew, edge_count, g->cities, total);
    timer_print(&t, "Prim's MST");
}

/* -----------------------------------------
   UNION-FIND (Disjoint Set Union)
   needed by Kruskal's
----------------------------------------- */

static int parent_uf[MAX_CITIES];
static int rank_uf[MAX_CITIES];

static void uf_init(int n) {
    for (int i = 0; i < n; i++) {
        parent_uf[i] = i;
        rank_uf[i]   = 0;
    }
}

static int uf_find(int x) {
    /* path compression */
    if (parent_uf[x] != x)
        parent_uf[x] = uf_find(parent_uf[x]);
    return parent_uf[x];
}

static void uf_union(int x, int y) {
    int rx = uf_find(x), ry = uf_find(y);
    if (rx == ry) return;
    /* union by rank */
    if (rank_uf[rx] < rank_uf[ry])      parent_uf[rx] = ry;
    else if (rank_uf[rx] > rank_uf[ry]) parent_uf[ry] = rx;
    else { parent_uf[ry] = rx; rank_uf[rx]++; }
}

/* -----------------------------------------
   ALGORITHM 2 -- KRUSKAL'S  O(E log E)
   Sorts all edges, adds cheapest that
   doesn't form a cycle. Best for sparse graphs.
----------------------------------------- */

typedef struct { int u, v, w; } Edge;

/* insertion sort on edges -- keeps it pure C, no qsort needed */
static void sort_edges(Edge edges[], int m) {
    for (int i = 1; i < m; i++) {
        Edge key = edges[i];
        int  j   = i - 1;
        while (j >= 0 && edges[j].w > key.w) {
            edges[j + 1] = edges[j];
            j--;
        }
        edges[j + 1] = key;
    }
}

void kruskal(Graph *g) {
    int  n = g->num_cities;
    Edge edges[MAX_CITIES * MAX_CITIES];
    int  m = 0;
    Timer t;

    /* extract unique edges from upper triangle of matrix */
    for (int u = 0; u < n; u++)
        for (int v = u + 1; v < n; v++)
            if (g->dist[u][v] != INF)
                edges[m++] = (Edge){u, v, g->dist[u][v]};

    timer_start(&t);

    sort_edges(edges, m);
    t.operations += m; /* count sort passes */

    uf_init(n);

    int eu[MAX_CITIES], ev[MAX_CITIES], ew[MAX_CITIES];
    int edge_count = 0, total = 0;

    for (int i = 0; i < m && edge_count < n - 1; i++) {
        t.operations++;
        int u = edges[i].u, v = edges[i].v;
        if (uf_find(u) != uf_find(v)) {
            uf_union(u, v);
            eu[edge_count] = u;
            ev[edge_count] = v;
            ew[edge_count] = edges[i].w;
            total += edges[i].w;
            edge_count++;
        }
    }

    timer_stop(&t);

    printf("\n  +--- KRUSKAL'S MST (Greedy -- Sparse Graph Optimal) ---+\n");
    printf("  |  Complexity  : O(E log E)  |  E = %d edges found\n", m);
    printf("  |  DSU used    : Union-by-Rank + Path Compression\n");
    printf("  |  Purpose     : Min-cost network to connect all cities\n");
    print_mst_edges(eu, ev, ew, edge_count, g->cities, total);
    timer_print(&t, "Kruskal's MST");
}
