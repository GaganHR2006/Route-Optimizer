/*
 * ALGORITHMS.H - Comprehensive Algorithm Library
 * Design and Analysis of Algorithms - Mini Project
 *
 * This header consolidates all algorithm declarations and provides
 * a unified interface for algorithm demonstrations.
 */

#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "graph.h"
#include "knapsack.h"

/* ═══════════════════════════════════════════════════════════════════════════
   ALGORITHM CATEGORIES - For educational organization
   ═══════════════════════════════════════════════════════════════════════════
 */

typedef enum {
  PARADIGM_GREEDY,
  PARADIGM_DYNAMIC_PROGRAMMING,
  PARADIGM_DIVIDE_CONQUER,
  PARADIGM_BACKTRACKING,
  PARADIGM_GRAPH_TRAVERSAL,
  PARADIGM_BRANCH_BOUND
} AlgorithmParadigm;

typedef struct {
  const char *name;
  const char *description;
  const char *complexity_time;
  const char *complexity_space;
  AlgorithmParadigm paradigm;
  const char *real_world_use;
} AlgorithmInfo;

/* ═══════════════════════════════════════════════════════════════════════════
   A* SEARCH ALGORITHM - Informed Search with Heuristics
   Real-world: GPS Navigation, Game AI Pathfinding
   ═══════════════════════════════════════════════════════════════════════════
 */

#define GRID_SIZE 15

typedef struct {
  int x, y;
} Point;

typedef struct {
  int grid[GRID_SIZE][GRID_SIZE]; // 0=free, 1=obstacle
  Point start;
  Point goal;
  int path[GRID_SIZE * GRID_SIZE][2];
  int path_length;
  int nodes_explored;
  int open_set_max;
} AStarGrid;

void astar_init_grid(AStarGrid *g);
void astar_add_obstacle(AStarGrid *g, int x, int y);
void astar_set_start(AStarGrid *g, int x, int y);
void astar_set_goal(AStarGrid *g, int x, int y);
int astar_find_path(AStarGrid *g);
void astar_demo(void);

/* ═══════════════════════════════════════════════════════════════════════════
   ACTIVITY SELECTION - Greedy Scheduling
   Real-world: Meeting Room Scheduling, CPU Task Scheduling
   ═══════════════════════════════════════════════════════════════════════════
 */

#define MAX_ACTIVITIES 20

typedef struct {
  char name[50];
  int start_time;
  int end_time;
  int priority; // For weighted version
} Activity;

typedef struct {
  Activity activities[MAX_ACTIVITIES];
  int count;
  int selected[MAX_ACTIVITIES];
  int selected_count;
} ActivitySchedule;

void activity_load_sample(ActivitySchedule *sched);
void activity_selection_greedy(ActivitySchedule *sched);
void activity_selection_weighted_dp(ActivitySchedule *sched);
void activity_demo(void);

/* ═══════════════════════════════════════════════════════════════════════════
   SORTING ALGORITHMS - Visualization & Comparison
   Real-world: Database indexing, Search optimization
   ═══════════════════════════════════════════════════════════════════════════
 */

#define MAX_SORT_SIZE 100

typedef struct {
  int data[MAX_SORT_SIZE];
  int size;
  long comparisons;
  long swaps;
  double time_ms;
} SortArray;

void sort_generate_random(SortArray *arr, int size);
void sort_generate_sorted(SortArray *arr, int size);
void sort_generate_reverse(SortArray *arr, int size);
void sort_generate_nearly_sorted(SortArray *arr, int size);

void heap_sort(SortArray *arr);
void merge_sort(SortArray *arr);
void quick_sort(SortArray *arr);
void insertion_sort(SortArray *arr);

void sorting_comparison_demo(void);

/* ═══════════════════════════════════════════════════════════════════════════
   HUFFMAN CODING - Greedy Compression
   Real-world: File compression (ZIP, GZIP), Image formats (JPEG)
   ═══════════════════════════════════════════════════════════════════════════
 */

#define MAX_CHARS 256

typedef struct HuffmanNode {
  char character;
  int frequency;
  struct HuffmanNode *left;
  struct HuffmanNode *right;
} HuffmanNode;

typedef struct {
  char character;
  char code[32];
  int frequency;
} HuffmanCode;

typedef struct {
  HuffmanCode codes[MAX_CHARS];
  int code_count;
  int original_bits;
  int compressed_bits;
  double compression_ratio;
} HuffmanResult;

void huffman_encode(const char *text, HuffmanResult *result);
void huffman_demo(void);

/* ═══════════════════════════════════════════════════════════════════════════
   LONGEST COMMON SUBSEQUENCE - Dynamic Programming
   Real-world: DNA sequence alignment, Diff tools, Version control
   ═══════════════════════════════════════════════════════════════════════════
 */

#define MAX_SEQ_LEN 100

typedef struct {
  char seq1[MAX_SEQ_LEN];
  char seq2[MAX_SEQ_LEN];
  char lcs[MAX_SEQ_LEN];
  int lcs_length;
  int dp_table[MAX_SEQ_LEN][MAX_SEQ_LEN];
  long operations;
} LCSResult;

void lcs_compute(const char *s1, const char *s2, LCSResult *result);
void lcs_demo(void);

/* ═══════════════════════════════════════════════════════════════════════════
   MATRIX CHAIN MULTIPLICATION - Dynamic Programming
   Real-world: Database query optimization, Graphics transformations
   ═══════════════════════════════════════════════════════════════════════════
 */

#define MAX_MATRICES 10

typedef struct {
  int dimensions[MAX_MATRICES + 1]; // n matrices need n+1 dimensions
  int count;
  int min_operations;
  char optimal_parenthesization[200];
  int dp_table[MAX_MATRICES][MAX_MATRICES];
} MatrixChainResult;

void matrix_chain_multiply(int dims[], int n, MatrixChainResult *result);
void matrix_chain_demo(void);

/* ═══════════════════════════════════════════════════════════════════════════
   ALGORITHM INFO DATABASE - For educational display
   ═══════════════════════════════════════════════════════════════════════════
 */

const AlgorithmInfo *get_algorithm_info(const char *name);
void print_algorithm_info(const AlgorithmInfo *info);
void print_all_algorithms(void);

#endif /* ALGORITHMS_H */
