/*
 * ALGORITHMS.C - Additional Algorithm Implementations
 * Design and Analysis of Algorithms - Mini Project
 */

#include "algorithms.h"
#include "benchmark.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Algorithm Database */
static const AlgorithmInfo ALGORITHM_DATABASE[] = {
    {"Dijkstra", "Single-source shortest path", "O(V^2)", "O(V)",
     PARADIGM_GREEDY, "GPS Navigation"},
    {"A* Search", "Heuristic-guided search", "O(E)-O(V^2)", "O(V)",
     PARADIGM_GREEDY, "Game AI"},
    {"Prim MST", "MST from vertex", "O(V^2)", "O(V)", PARADIGM_GREEDY,
     "Network Design"},
    {"Kruskal MST", "MST by edge sorting", "O(E log E)", "O(V)",
     PARADIGM_GREEDY, "Clustering"},
    {"0/1 Knapsack", "Subset selection", "O(n*W)", "O(n*W)",
     PARADIGM_DYNAMIC_PROGRAMMING, "Cargo Loading"},
    {"Activity Selection", "Max non-overlapping", "O(n log n)", "O(n)",
     PARADIGM_GREEDY, "Scheduling"},
    {"TSP B&B", "Optimal tour", "O(n!) pruned", "O(n)", PARADIGM_BRANCH_BOUND,
     "Delivery Routes"},
    {"Heap Sort", "Heap-based sorting", "O(n log n)", "O(1)",
     PARADIGM_DIVIDE_CONQUER, "Priority Queues"},
    {"LCS", "Longest common subseq", "O(m*n)", "O(m*n)",
     PARADIGM_DYNAMIC_PROGRAMMING, "DNA Alignment"},
    {NULL, NULL, NULL, NULL, 0, NULL}};

const AlgorithmInfo *get_algorithm_info(const char *name) {
  for (int i = 0; ALGORITHM_DATABASE[i].name != NULL; i++)
    if (strcmp(ALGORITHM_DATABASE[i].name, name) == 0)
      return &ALGORITHM_DATABASE[i];
  return NULL;
}

static const char *paradigm_name(AlgorithmParadigm p) {
  switch (p) {
  case PARADIGM_GREEDY:
    return "Greedy";
  case PARADIGM_DYNAMIC_PROGRAMMING:
    return "Dynamic Programming";
  case PARADIGM_DIVIDE_CONQUER:
    return "Divide & Conquer";
  case PARADIGM_BACKTRACKING:
    return "Backtracking";
  case PARADIGM_GRAPH_TRAVERSAL:
    return "Graph Traversal";
  case PARADIGM_BRANCH_BOUND:
    return "Branch & Bound";
  default:
    return "Unknown";
  }
}

void print_algorithm_info(const AlgorithmInfo *info) {
  if (!info)
    return;
  printf(
      "\n  +==============================================================+\n");
  printf("  |  ALGORITHM: %-46s  |\n", info->name);
  printf("  |  Paradigm: %-45s  |\n", paradigm_name(info->paradigm));
  printf("  |  Time: %-48s  |\n", info->complexity_time);
  printf("  |  Use: %-49s  |\n", info->real_world_use);
  printf(
      "  +==============================================================+\n");
}

void print_all_algorithms(void) {
  printf("\n  "
         "+===================================================================="
         "========+\n");
  printf("  |                    ALGORITHM REFERENCE GUIDE                     "
         "         |\n");
  printf("  "
         "+--------------------------------------------------------------------"
         "--------+\n");
  for (int i = 0; ALGORITHM_DATABASE[i].name != NULL; i++) {
    const AlgorithmInfo *a = &ALGORITHM_DATABASE[i];
    printf("  |  %-16s | %-18s | %-12s | %-14s  |\n", a->name,
           paradigm_name(a->paradigm), a->complexity_time, a->real_world_use);
  }
  printf("  "
         "+===================================================================="
         "========+\n");
}

/* A* SEARCH */
static int heuristic(int x1, int y1, int x2, int y2) {
  int dx = x1 - x2;
  if (dx < 0)
    dx = -dx;
  int dy = y1 - y2;
  if (dy < 0)
    dy = -dy;
  return dx + dy;
}

void astar_init_grid(AStarGrid *g) {
  for (int i = 0; i < GRID_SIZE; i++)
    for (int j = 0; j < GRID_SIZE; j++)
      g->grid[i][j] = 0;
  g->start.x = 0;
  g->start.y = 0;
  g->goal.x = GRID_SIZE - 1;
  g->goal.y = GRID_SIZE - 1;
  g->path_length = 0;
  g->nodes_explored = 0;
  g->open_set_max = 0;
}

void astar_add_obstacle(AStarGrid *g, int x, int y) {
  if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE)
    g->grid[x][y] = 1;
}

void astar_set_start(AStarGrid *g, int x, int y) {
  g->start.x = x;
  g->start.y = y;
}
void astar_set_goal(AStarGrid *g, int x, int y) {
  g->goal.x = x;
  g->goal.y = y;
}

int astar_find_path(AStarGrid *g) {
  typedef struct {
    int x, y, g_cost, f_cost;
  } Node;
  Node open[GRID_SIZE * GRID_SIZE];
  int oc = 0, closed[GRID_SIZE][GRID_SIZE], gc[GRID_SIZE][GRID_SIZE];
  int px[GRID_SIZE][GRID_SIZE], py[GRID_SIZE][GRID_SIZE];

  for (int i = 0; i < GRID_SIZE; i++)
    for (int j = 0; j < GRID_SIZE; j++) {
      closed[i][j] = 0;
      gc[i][j] = INT_MAX;
      px[i][j] = -1;
      py[i][j] = -1;
    }

  gc[g->start.x][g->start.y] = 0;
  open[oc].x = g->start.x;
  open[oc].y = g->start.y;
  open[oc].g_cost = 0;
  open[oc].f_cost = heuristic(g->start.x, g->start.y, g->goal.x, g->goal.y);
  oc++;

  int dx[] = {0, 1, 0, -1}, dy[] = {1, 0, -1, 0};
  g->nodes_explored = 0;
  g->open_set_max = 1;

  while (oc > 0) {
    int best = 0;
    for (int i = 1; i < oc; i++)
      if (open[i].f_cost < open[best].f_cost)
        best = i;
    Node cur = open[best];
    open[best] = open[--oc];

    if (cur.x == g->goal.x && cur.y == g->goal.y) {
      int cx = g->goal.x, cy = g->goal.y;
      g->path_length = 0;
      while (cx != -1 && cy != -1) {
        g->path[g->path_length][0] = cx;
        g->path[g->path_length][1] = cy;
        g->path_length++;
        int tx = px[cx][cy], ty = py[cx][cy];
        cx = tx;
        cy = ty;
      }
      return 1;
    }

    closed[cur.x][cur.y] = 1;
    g->nodes_explored++;

    for (int d = 0; d < 4; d++) {
      int nx = cur.x + dx[d], ny = cur.y + dy[d];
      if (nx < 0 || nx >= GRID_SIZE || ny < 0 || ny >= GRID_SIZE)
        continue;
      if (g->grid[nx][ny] == 1 || closed[nx][ny])
        continue;
      int tg = gc[cur.x][cur.y] + 1;
      if (tg < gc[nx][ny]) {
        gc[nx][ny] = tg;
        px[nx][ny] = cur.x;
        py[nx][ny] = cur.y;
        int found = 0;
        for (int i = 0; i < oc; i++)
          if (open[i].x == nx && open[i].y == ny) {
            open[i].f_cost = tg + heuristic(nx, ny, g->goal.x, g->goal.y);
            found = 1;
            break;
          }
        if (!found) {
          open[oc].x = nx;
          open[oc].y = ny;
          open[oc].g_cost = tg;
          open[oc].f_cost = tg + heuristic(nx, ny, g->goal.x, g->goal.y);
          oc++;
          if (oc > g->open_set_max)
            g->open_set_max = oc;
        }
      }
    }
  }
  return 0;
}

void astar_demo(void) {
  AStarGrid grid;
  Timer t;
  printf(
      "\n  +==============================================================+\n");
  printf(
      "  |           A* SEARCH - ROBOT PATH PLANNING DEMO               |\n");
  printf(
      "  +==============================================================+\n");
  printf(
      "  |  Navigate from (0,0) to (14,14) avoiding obstacles.          |\n");
  printf(
      "  |  A* uses heuristics to explore fewer nodes than Dijkstra!    |\n");
  printf(
      "  +==============================================================+\n");

  astar_init_grid(&grid);
  for (int i = 2; i < 12; i++)
    astar_add_obstacle(&grid, i, 3);
  for (int i = 4; i < 14; i++)
    astar_add_obstacle(&grid, i, 7);
  for (int i = 1; i < 11; i++)
    astar_add_obstacle(&grid, i, 11);

  printf("\n  Grid (S=Start, G=Goal, #=Obstacle):\n");
  for (int i = 0; i < GRID_SIZE; i++) {
    printf("  ");
    for (int j = 0; j < GRID_SIZE; j++) {
      if (i == grid.start.x && j == grid.start.y)
        printf("S");
      else if (i == grid.goal.x && j == grid.goal.y)
        printf("G");
      else if (grid.grid[i][j])
        printf("#");
      else
        printf(".");
    }
    printf("\n");
  }

  timer_start(&t);
  int found = astar_find_path(&grid);
  timer_stop(&t);

  if (found) {
    int pg[GRID_SIZE][GRID_SIZE] = {{0}};
    for (int p = 0; p < grid.path_length; p++)
      pg[grid.path[p][0]][grid.path[p][1]] = 1;
    printf("\n  Solution (* = path):\n");
    for (int i = 0; i < GRID_SIZE; i++) {
      printf("  ");
      for (int j = 0; j < GRID_SIZE; j++) {
        if (i == grid.start.x && j == grid.start.y)
          printf("S");
        else if (i == grid.goal.x && j == grid.goal.y)
          printf("G");
        else if (grid.grid[i][j])
          printf("#");
        else if (pg[i][j])
          printf("*");
        else
          printf(".");
      }
      printf("\n");
    }
    printf("\n  Path: %d steps, Nodes explored: %d\n", grid.path_length - 1,
           grid.nodes_explored);
  }
  timer_print(&t, "A* Search");
}

/* ACTIVITY SELECTION */
void activity_load_sample(ActivitySchedule *s) {
  s->count = 8;
  strcpy(s->activities[0].name, "Standup");
  s->activities[0].start_time = 9;
  s->activities[0].end_time = 10;
  strcpy(s->activities[1].name, "Client");
  s->activities[1].start_time = 9;
  s->activities[1].end_time = 11;
  strcpy(s->activities[2].name, "Review");
  s->activities[2].start_time = 10;
  s->activities[2].end_time = 12;
  strcpy(s->activities[3].name, "Lunch");
  s->activities[3].start_time = 12;
  s->activities[3].end_time = 13;
  strcpy(s->activities[4].name, "Planning");
  s->activities[4].start_time = 11;
  s->activities[4].end_time = 14;
  strcpy(s->activities[5].name, "1-on-1");
  s->activities[5].start_time = 14;
  s->activities[5].end_time = 15;
  strcpy(s->activities[6].name, "Demo");
  s->activities[6].start_time = 15;
  s->activities[6].end_time = 17;
  strcpy(s->activities[7].name, "Retro");
  s->activities[7].start_time = 16;
  s->activities[7].end_time = 18;
  for (int i = 0; i < s->count; i++) {
    s->selected[i] = 0;
    s->activities[i].priority = 5;
  }
  s->selected_count = 0;
}

void activity_selection_greedy(ActivitySchedule *s) {
  Activity sorted[MAX_ACTIVITIES];
  int idx[MAX_ACTIVITIES];
  for (int i = 0; i < s->count; i++) {
    sorted[i] = s->activities[i];
    idx[i] = i;
  }
  for (int i = 0; i < s->count - 1; i++)
    for (int j = 0; j < s->count - i - 1; j++)
      if (sorted[j].end_time > sorted[j + 1].end_time) {
        Activity t = sorted[j];
        sorted[j] = sorted[j + 1];
        sorted[j + 1] = t;
        int ti = idx[j];
        idx[j] = idx[j + 1];
        idx[j + 1] = ti;
      }
  for (int i = 0; i < s->count; i++)
    s->selected[idx[i]] = 0;
  s->selected_count = 0;
  int last = 0;
  for (int i = 0; i < s->count; i++)
    if (sorted[i].start_time >= last) {
      s->selected[idx[i]] = 1;
      s->selected_count++;
      last = sorted[i].end_time;
    }
  printf("\n  Greedy Selection (by earliest end time):\n");
  for (int i = 0; i < s->count; i++)
    printf("  %-10s %2d:00-%2d:00 %s\n", sorted[i].name, sorted[i].start_time,
           sorted[i].end_time, s->selected[idx[i]] ? "[SELECTED]" : "");
  printf("  Selected: %d/%d\n", s->selected_count, s->count);
}

void activity_selection_weighted_dp(ActivitySchedule *s) { (void)s; }

void activity_demo(void) {
  ActivitySchedule s;
  printf(
      "\n  +==============================================================+\n");
  printf(
      "  |        ACTIVITY SELECTION - MEETING SCHEDULING               |\n");
  printf(
      "  +==============================================================+\n");
  activity_load_sample(&s);
  activity_selection_greedy(&s);
}

/* SORTING */
void sort_generate_random(SortArray *a, int n) {
  a->size = n;
  a->comparisons = 0;
  a->swaps = 0;
  a->time_ms = 0;
  for (int i = 0; i < n; i++)
    a->data[i] = rand() % 1000;
}
void sort_generate_sorted(SortArray *a, int n) {
  a->size = n;
  a->comparisons = 0;
  a->swaps = 0;
  a->time_ms = 0;
  for (int i = 0; i < n; i++)
    a->data[i] = i;
}
void sort_generate_reverse(SortArray *a, int n) {
  a->size = n;
  a->comparisons = 0;
  a->swaps = 0;
  a->time_ms = 0;
  for (int i = 0; i < n; i++)
    a->data[i] = n - i;
}
void sort_generate_nearly_sorted(SortArray *a, int n) {
  sort_generate_sorted(a, n);
  for (int i = 0; i < n / 10; i++) {
    int j = rand() % n, k = rand() % n, t = a->data[j];
    a->data[j] = a->data[k];
    a->data[k] = t;
  }
}

static void heapify(SortArray *a, int n, int i) {
  int lg = i, l = 2 * i + 1, r = 2 * i + 2;
  a->comparisons++;
  if (l < n && a->data[l] > a->data[lg])
    lg = l;
  a->comparisons++;
  if (r < n && a->data[r] > a->data[lg])
    lg = r;
  if (lg != i) {
    int t = a->data[i];
    a->data[i] = a->data[lg];
    a->data[lg] = t;
    a->swaps++;
    heapify(a, n, lg);
  }
}

void heap_sort(SortArray *a) {
  Timer t;
  timer_start(&t);
  for (int i = a->size / 2 - 1; i >= 0; i--)
    heapify(a, a->size, i);
  for (int i = a->size - 1; i > 0; i--) {
    int t = a->data[0];
    a->data[0] = a->data[i];
    a->data[i] = t;
    a->swaps++;
    heapify(a, i, 0);
  }
  timer_stop(&t);
  a->time_ms = timer_elapsed_ms(&t);
}

static void merge_a(SortArray *a, int l, int m, int r) {
  int n1 = m - l + 1, n2 = r - m, L[MAX_SORT_SIZE], R[MAX_SORT_SIZE];
  for (int i = 0; i < n1; i++)
    L[i] = a->data[l + i];
  for (int j = 0; j < n2; j++)
    R[j] = a->data[m + 1 + j];
  int i = 0, j = 0, k = l;
  while (i < n1 && j < n2) {
    a->comparisons++;
    a->data[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
  }
  while (i < n1)
    a->data[k++] = L[i++];
  while (j < n2)
    a->data[k++] = R[j++];
}
static void merge_r(SortArray *a, int l, int r) {
  if (l < r) {
    int m = l + (r - l) / 2;
    merge_r(a, l, m);
    merge_r(a, m + 1, r);
    merge_a(a, l, m, r);
  }
}
void merge_sort(SortArray *a) {
  Timer t;
  timer_start(&t);
  merge_r(a, 0, a->size - 1);
  timer_stop(&t);
  a->time_ms = timer_elapsed_ms(&t);
}

static int part(SortArray *a, int lo, int hi) {
  int pv = a->data[hi], i = lo - 1;
  for (int j = lo; j < hi; j++) {
    a->comparisons++;
    if (a->data[j] < pv) {
      i++;
      int t = a->data[i];
      a->data[i] = a->data[j];
      a->data[j] = t;
      a->swaps++;
    }
  }
  int t = a->data[i + 1];
  a->data[i + 1] = a->data[hi];
  a->data[hi] = t;
  a->swaps++;
  return i + 1;
}
static void quick_r(SortArray *a, int lo, int hi) {
  if (lo < hi) {
    int p = part(a, lo, hi);
    quick_r(a, lo, p - 1);
    quick_r(a, p + 1, hi);
  }
}
void quick_sort(SortArray *a) {
  Timer t;
  timer_start(&t);
  quick_r(a, 0, a->size - 1);
  timer_stop(&t);
  a->time_ms = timer_elapsed_ms(&t);
}

void insertion_sort(SortArray *a) {
  Timer t;
  timer_start(&t);
  for (int i = 1; i < a->size; i++) {
    int k = a->data[i], j = i - 1;
    while (j >= 0 && a->data[j] > k) {
      a->comparisons++;
      a->data[j + 1] = a->data[j];
      a->swaps++;
      j--;
    }
    a->comparisons++;
    a->data[j + 1] = k;
  }
  timer_stop(&t);
  a->time_ms = timer_elapsed_ms(&t);
}

void sorting_comparison_demo(void) {
  printf(
      "\n  +==============================================================+\n");
  printf(
      "  |              SORTING ALGORITHM COMPARISON                    |\n");
  printf(
      "  +==============================================================+\n");
  int n = 100;
  SortArray h, m, q, ins;
  sort_generate_random(&h, n);
  memcpy(m.data, h.data, sizeof(int) * (size_t)n);
  m.size = n;
  m.comparisons = 0;
  m.swaps = 0;
  memcpy(q.data, h.data, sizeof(int) * (size_t)n);
  q.size = n;
  q.comparisons = 0;
  q.swaps = 0;
  memcpy(ins.data, h.data, sizeof(int) * (size_t)n);
  ins.size = n;
  ins.comparisons = 0;
  ins.swaps = 0;
  heap_sort(&h);
  merge_sort(&m);
  quick_sort(&q);
  insertion_sort(&ins);
  printf("\n  Size: %d elements\n", n);
  printf("  %-12s | %10s | %10s\n", "Algorithm", "Compares", "Swaps");
  printf("  ------------|------------|------------\n");
  printf("  %-12s | %10ld | %10ld\n", "Heap Sort", h.comparisons, h.swaps);
  printf("  %-12s | %10ld | %10ld\n", "Merge Sort", m.comparisons, m.swaps);
  printf("  %-12s | %10ld | %10ld\n", "Quick Sort", q.comparisons, q.swaps);
  printf("  %-12s | %10ld | %10ld\n", "Insertion", ins.comparisons, ins.swaps);
  printf("\n  O(n log n) sorts beat O(n^2) as n grows!\n");
}

/* LCS */
void lcs_compute(const char *s1, const char *s2, LCSResult *r) {
  strncpy(r->seq1, s1, MAX_SEQ_LEN - 1);
  strncpy(r->seq2, s2, MAX_SEQ_LEN - 1);
  int m = (int)strlen(s1), n = (int)strlen(s2);
  r->operations = 0;
  for (int i = 0; i <= m; i++)
    r->dp_table[i][0] = 0;
  for (int j = 0; j <= n; j++)
    r->dp_table[0][j] = 0;
  for (int i = 1; i <= m; i++)
    for (int j = 1; j <= n; j++) {
      r->operations++;
      if (s1[i - 1] == s2[j - 1])
        r->dp_table[i][j] = r->dp_table[i - 1][j - 1] + 1;
      else
        r->dp_table[i][j] = r->dp_table[i - 1][j] > r->dp_table[i][j - 1]
                                ? r->dp_table[i - 1][j]
                                : r->dp_table[i][j - 1];
    }
  r->lcs_length = r->dp_table[m][n];
  int idx = r->lcs_length;
  r->lcs[idx] = '\0';
  int i = m, j = n;
  while (i > 0 && j > 0) {
    if (s1[i - 1] == s2[j - 1]) {
      r->lcs[--idx] = s1[i - 1];
      i--;
      j--;
    } else if (r->dp_table[i - 1][j] > r->dp_table[i][j - 1])
      i--;
    else
      j--;
  }
}

void lcs_demo(void) {
  LCSResult r;
  printf(
      "\n  +==============================================================+\n");
  printf(
      "  |     LONGEST COMMON SUBSEQUENCE - DNA SEQUENCE ALIGNMENT      |\n");
  printf(
      "  +==============================================================+\n");
  lcs_compute("AGGTAB", "GXTXAYB", &r);
  printf("  Sequence 1: %s\n", r.seq1);
  printf("  Sequence 2: %s\n", r.seq2);
  printf("  LCS: %s (length %d)\n", r.lcs, r.lcs_length);
  printf("  Operations: %ld\n", r.operations);
}

/* Huffman stubs */
void huffman_encode(const char *text, HuffmanResult *r) {
  (void)text;
  r->compression_ratio = 0.5;
}
void huffman_demo(void) { printf("\n  Huffman Coding Demo\n"); }

/* Matrix Chain stubs */
void matrix_chain_multiply(int dims[], int n, MatrixChainResult *r) {
  (void)dims;
  (void)n;
  r->min_operations = 0;
}
void matrix_chain_demo(void) { printf("\n  Matrix Chain Demo\n"); }
