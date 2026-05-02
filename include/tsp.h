#ifndef TSP_H
#define TSP_H

#include "graph.h"

/* Runs brute-force first (baseline), then Branch & Bound,
   prints both results and the node-count comparison table. */
void tsp_branch_and_bound(Graph *g);

#endif
