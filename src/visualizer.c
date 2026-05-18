#include "visualizer.h"
#include "graph.h"
#include "knapsack.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>

#define WINDOW_W 1400
#define WINDOW_H 800
#define CANVAS_W 800
#define INFO_W 600
#define STORY_H 100
#define AUTO_DELAY 2.0f

static const Color C_BG = {15, 20, 30, 255};
static const Color C_PANEL = {25, 32, 45, 255};
static const Color C_CARD = {35, 45, 65, 255};
static const Color C_EDGE = {70, 80, 100, 255};
static const Color C_NODE = {60, 90, 150, 255};
static const Color C_VISIT = {40, 200, 100, 255};
static const Color C_CURR = {255, 200, 50, 255};
static const Color C_PATH = {50, 200, 255, 255};
static const Color C_MST_E = {50, 230, 120, 255};
static const Color C_REJECT = {220, 70, 70, 255};
static const Color C_BF = {230, 80, 80, 255};
static const Color C_BB = {80, 230, 120, 255};
static const Color C_DIM = {130, 140, 160, 255};
static const Color C_ORANGE = {255, 170, 50, 255};
static const Color C_CYAN = {50, 200, 255, 255};
static const Color C_GOLD = {255, 220, 80, 255};
static const Color C_WHITE = {255, 255, 255, 255};

static const Vector2 POS[6] = {{380, 520}, {580, 490}, {430, 340},
                               {180, 340}, {220, 440}, {400, 120}};

typedef enum {
  SCREEN_MENU,
  SCREEN_DIJKSTRA_INTRO,
  SCREEN_DIJKSTRA,
  SCREEN_KRUSKAL_INTRO,
  SCREEN_KRUSKAL,
  SCREEN_TSP_INTRO,
  SCREEN_TSP,
  SCREEN_KNAPSACK_INTRO,
  SCREEN_KNAPSACK
} ScreenState;

static const char *DIJKSTRA_STORIES[] = {
    "SCENARIO: You're a logistics manager. Find cheapest routes from Bangalore "
    "warehouse to all cities!\n"
    "GREEDY STRATEGY: Always expand to the CLOSEST unvisited city. This "
    "guarantees optimal paths!",

    "STEP 1: Bangalore LOCKED at 0 km (our starting warehouse).\n"
    "Discovered roads: Chennai=346km, Hyderabad=575km, Mumbai=984km. Which is "
    "closest?",

    "STEP 2: Chennai is CLOSEST (346 km) - LOCKED! Why? No other path can be "
    "shorter!\n"
    "This is the GREEDY CHOICE that makes Dijkstra optimal. We never revisit "
    "locked cities.",

    "STEP 3: Hyderabad LOCKED at 575 km (direct from Bangalore).\n"
    "Chennai->Hyderabad = 346+627 = 973 km - WORSE! Greedy keeps the shorter "
    "direct path.",

    "STEP 4: Mumbai LOCKED at 984 km. Now Pune is reachable: 984+149 = 1133 "
    "km.\n"
    "Notice: We're building a shortest-path TREE from Bangalore to all cities!",

    "STEP 5: Pune LOCKED at 1133 km. Delhi reachable via Pune (2546km) or "
    "Hyderabad (2143km).\n"
    "Hyderabad route wins! This shows why we need to check ALL paths.",

    "COMPLETE! All cities reached with OPTIMAL distances. Complexity: O(V^2) = "
    "36 operations.\n"
    "REAL-WORLD USE: GPS navigation, network routing, social network "
    "analysis!"};

static const char *KRUSKAL_STORIES[] = {
    "SCENARIO: You're a telecom engineer. Lay fiber cables to connect ALL "
    "cities with MINIMUM cost!\n"
    "GREEDY STRATEGY: Sort edges by weight, add cheapest edge that doesn't "
    "create a cycle.",

    "STEP 1: Mumbai-Pune (149 km) - CHEAPEST edge! ADDED to MST.\n"
    "MST Cost: 149 km. Two cities connected, four more to go!",

    "STEP 2: Bangalore-Chennai (346 km) - ADDED! Creates a second component.\n"
    "MST Cost: 495 km. We now have TWO separate tree components.",

    "STEP 3: Bangalore-Hyderabad (575 km) - ADDED! Extends Bangalore's "
    "component.\n"
    "MST Cost: 1070 km. Bangalore component now has 3 cities!",

    "STEP 4: Chennai-Hyderabad (627 km) - REJECTED! Would create CYCLE!\n"
    "Both cities already connected. Union-Find detects this in O(alpha(n)) "
    "time!",

    "STEP 5: Hyderabad-Mumbai (711 km) - ADDED! MERGES the two components!\n"
    "MST Cost: 1781 km. Now 5 cities in one connected tree!",

    "STEP 6: Bangalore-Mumbai (984 km) - REJECTED! Would create CYCLE!\n"
    "Already connected through Hyderabad. Greedy skips redundant edges.",

    "STEP 7: Pune-Delhi (1413 km) - ADDED! Final edge connects Delhi!\n"
    "MST Cost: 3194 km. ALL 6 cities connected with MINIMUM total cable!",

    "COMPLETE! MST has exactly V-1 = 5 edges. Complexity: O(E log E) for "
    "sorting.\n"
    "REAL-WORLD USE: Network design, circuit layout, cluster analysis!"};

typedef struct {
  int u, v, weight, added;
} Edge;
static Edge EDGES[] = {{3, 4, 149, 0},  {0, 1, 346, 0}, {0, 2, 575, 0},
                       {1, 2, 627, 0},  {2, 3, 711, 0}, {0, 3, 984, 0},
                       {4, 5, 1413, 0}, {2, 5, 1568, 0}};
#define NUM_EDGES 8

static int dijk_dist[6], dijk_visited[6], dijk_step = 0, dijk_current = -1;
static int kruskal_step = 0;
static int tsp_bf_tour[MAX_CITIES], tsp_bb_tour[MAX_CITIES];
static int tsp_bf_cost = 0, tsp_bb_cost = 0, tsp_bf_nodes = 0, tsp_bb_nodes = 0;
static long tsp_bb_pruned = 0;
static int tsp_computed = 0, tsp_mode = 0;
static int knapsack_capacity = 50;
static int knapsack_dp_selected[MAX_ITEMS], knapsack_dp_total_weight = 0,
                                            knapsack_dp_total_value = 0;
static double knapsack_greedy_fraction[MAX_ITEMS],
    knapsack_greedy_total_value = 0.0;
static int knapsack_greedy_total_weight = 0, knapsack_computed = 0,
           knapsack_mode = 0;
static CargoItem knapsack_sorted[MAX_ITEMS];
static ScreenState current_screen = SCREEN_MENU;
static Graph graph;
static CargoItem items[MAX_ITEMS];
static int num_items = 0, auto_advance = 0;
static float auto_timer = 0.0f;

static void draw_text_centered(const char *t, int x, int y, int s, Color c) {
  DrawText(t, x - MeasureText(t, s) / 2, y, s, c);
}

static void draw_button(Rectangle r, const char *t, int sel, Color a) {
  DrawRectangleRounded(r, 0.2f, 8, sel ? a : C_CARD);
  DrawRectangleRoundedLines(r, 0.2f, 8, a);
  DrawText(t, (int)(r.x + (r.width - MeasureText(t, 24)) / 2),
           (int)(r.y + (r.height - 24) / 2), 24, sel ? C_BG : a);
}

static void draw_story_bar(const char *text) {
  DrawRectangle(0, WINDOW_H - STORY_H, WINDOW_W, STORY_H, C_PANEL);
  DrawRectangleLinesEx((Rectangle){0, WINDOW_H - STORY_H, WINDOW_W, STORY_H}, 3,
                       C_EDGE);
  DrawText(text, 30, WINDOW_H - STORY_H + 20, 24, C_WHITE);
}

static void reset_dijkstra(void) {
  dijk_step = 0;
  dijk_current = -1;
  for (int i = 0; i < 6; i++) {
    dijk_dist[i] = (i == 0) ? 0 : INF;
    dijk_visited[i] = 0;
  }
}

static void reset_kruskal(void) {
  kruskal_step = 0;
  for (int i = 0; i < NUM_EDGES; i++)
    EDGES[i].added = 0;
}

static void draw_menu(void) {
  DrawRectangle(0, 0, WINDOW_W, WINDOW_H, C_BG);
  draw_text_centered("ALGORITHM VISUALIZER", WINDOW_W / 2, 40, 52, C_GOLD);
  draw_text_centered("Design & Analysis of Algorithms - Mini Project",
                     WINDOW_W / 2, 100, 24, C_DIM);
  draw_text_centered("See how algorithms solve REAL-WORLD problems!",
                     WINDOW_W / 2, 135, 22, C_CYAN);

  const char *titles[] = {"GPS Navigation", "Network Design", "Delivery Route",
                          "Cargo Loading"};
  const char *algos[] = {"Dijkstra's Algorithm", "Kruskal's MST",
                         "TSP: BF vs B&B", "0/1 Knapsack DP"};
  const char *problems[] = {"Find shortest routes", "Minimize cable cost",
                            "Optimal tour planning", "Maximize truck value"};
  const char *paradigms[] = {"GREEDY", "GREEDY", "BRANCH & BOUND",
                             "DYNAMIC PROG"};
  Color colors[] = {C_CYAN, C_MST_E, C_ORANGE, C_GOLD};

  float cw = 310, ch = 280, gap = 25, sx = (WINDOW_W - (4 * cw + 3 * gap)) / 2,
        y = 180;
  for (int i = 0; i < 4; i++) {
    Rectangle card = {sx + i * (cw + gap), y, cw, ch};
    DrawRectangleRounded(card, 0.1f, 8, C_CARD);
    DrawRectangleRoundedLines(card, 0.1f, 8, colors[i]);

    /* Number badge */
    DrawCircle((int)(card.x + 40), (int)(card.y + 40), 28, colors[i]);
    char num[4];
    snprintf(num, sizeof(num), "%d", i + 1);
    draw_text_centered(num, (int)(card.x + 40), (int)(card.y + 27), 32, C_BG);

    /* Title and algorithm */
    DrawText(titles[i], (int)(card.x + 20), (int)(card.y + 85), 26, C_WHITE);
    DrawText(algos[i], (int)(card.x + 20), (int)(card.y + 118), 20, colors[i]);

    /* Separator line */
    DrawRectangle((int)(card.x + 15), (int)(card.y + 150), (int)(cw - 30), 2,
                  C_EDGE);

    /* Problem description */
    DrawText("Real-World Problem:", (int)(card.x + 20), (int)(card.y + 165), 16,
             C_DIM);
    DrawText(problems[i], (int)(card.x + 20), (int)(card.y + 188), 18, C_WHITE);

    /* Paradigm badge */
    DrawRectangleRounded((Rectangle){card.x + 20, card.y + 218, 120, 24}, 0.3f,
                         4, colors[i]);
    DrawText(paradigms[i], (int)(card.x + 28), (int)(card.y + 222), 14, C_BG);

    /* Press hint */
    char hint[32];
    snprintf(hint, sizeof(hint), "Press %d to explore", i + 1);
    DrawText(hint, (int)(card.x + 20), (int)(card.y + 252), 18, C_DIM);
  }

  /* Footer with paradigm legend */
  DrawText("PARADIGMS:", 30, WINDOW_H - 80, 18, C_WHITE);
  DrawCircle(130, WINDOW_H - 71, 8, C_CYAN);
  DrawText("Greedy", 145, WINDOW_H - 80, 16, C_DIM);
  DrawCircle(220, WINDOW_H - 71, 8, C_GOLD);
  DrawText("Dynamic Programming", 235, WINDOW_H - 80, 16, C_DIM);
  DrawCircle(410, WINDOW_H - 71, 8, C_ORANGE);
  DrawText("Branch & Bound", 425, WINDOW_H - 80, 16, C_DIM);

  DrawText("Press ESC to exit | Press 1-4 to select an algorithm", 30,
           WINDOW_H - 45, 22, C_DIM);
}

static void draw_intro(const char *title, const char *prob, const char *sol,
                       Color a) {
  DrawRectangle(0, 0, WINDOW_W, WINDOW_H, C_BG);

  /* Title with underline */
  draw_text_centered(title, WINDOW_W / 2, 50, 44, a);
  DrawRectangle(WINDOW_W / 2 - 200, 100, 400, 3, a);

  /* Problem card */
  Rectangle p = {80, 130, WINDOW_W - 160, 170};
  DrawRectangleRounded(p, 0.05f, 8, C_CARD);
  DrawRectangleRoundedLines(p, 0.05f, 8, C_ORANGE);
  DrawCircle(130, 165, 20, C_ORANGE);
  DrawText("?", 123, 152, 28, C_BG);
  DrawText("THE REAL-WORLD PROBLEM", 160, 155, 26, C_ORANGE);
  DrawText(prob, 110, 200, 22, C_WHITE);

  /* Solution card */
  Rectangle s = {80, 320, WINDOW_W - 160, 170};
  DrawRectangleRounded(s, 0.05f, 8, C_CARD);
  DrawRectangleRoundedLines(s, 0.05f, 8, C_VISIT);
  DrawCircle(130, 355, 20, C_VISIT);
  DrawText("!", 125, 342, 28, C_BG);
  DrawText("THE ALGORITHMIC SOLUTION", 160, 345, 26, C_VISIT);
  DrawText(sol, 110, 390, 22, C_WHITE);

  /* Controls hint box */
  Rectangle c = {80, 510, WINDOW_W - 160, 80};
  DrawRectangleRounded(c, 0.05f, 8, C_CARD);
  DrawRectangleRoundedLines(c, 0.05f, 8, C_CYAN);
  DrawText("CONTROLS:", 110, 530, 20, C_CYAN);
  DrawText("SPACE = Step through algorithm    A = Auto-advance    R = Reset    "
           "ESC = Back",
           110, 560, 18, C_DIM);

  /* Start button */
  Rectangle btn = {WINDOW_W / 2 - 150, WINDOW_H - 120, 300, 50};
  DrawRectangleRounded(btn, 0.2f, 8, a);
  draw_text_centered("Press ENTER to Start", WINDOW_W / 2, WINDOW_H - 105, 24,
                     C_BG);

  DrawText("Press ESC to go back to menu", 30, WINDOW_H - 45, 20, C_DIM);
}

static void step_dijkstra(void) {
  if (dijk_step == 0) {
    dijk_current = 0;
    dijk_step++;
    return;
  }
  if (dijk_step <= 6) {
    if (dijk_current >= 0)
      dijk_visited[dijk_current] = 1;
    for (int i = 0; i < graph.num_cities; i++) {
      if (!dijk_visited[i] && graph.dist[dijk_current][i] < INF) {
        int nd = dijk_dist[dijk_current] + graph.dist[dijk_current][i];
        if (nd < dijk_dist[i])
          dijk_dist[i] = nd;
      }
    }
    int min_d = INF, next = -1;
    for (int i = 0; i < graph.num_cities; i++) {
      if (!dijk_visited[i] && dijk_dist[i] < min_d) {
        min_d = dijk_dist[i];
        next = i;
      }
    }
    dijk_current = next;
    dijk_step++;
  }
}

static void draw_dijkstra(void) {
  DrawRectangle(0, 0, WINDOW_W, WINDOW_H - STORY_H, C_BG);

  /* Draw title bar with algorithm info */
  DrawRectangle(0, 0, CANVAS_W, 50, C_PANEL);
  DrawText("SHORTEST PATH VISUALIZATION", 20, 12, 28, C_CYAN);
  char step_info[64];
  snprintf(step_info, sizeof(step_info), "Step %d/6", dijk_step);
  DrawText(step_info, CANVAS_W - 120, 15, 22, C_GOLD);

  /* Draw edges with weight labels in boxes */
  for (int i = 0; i < graph.num_cities; i++) {
    for (int j = i + 1; j < graph.num_cities; j++) {
      if (graph.dist[i][j] < INF) {
        int both_visited = dijk_visited[i] && dijk_visited[j];
        int one_current = (i == dijk_current || j == dijk_current);
        Color ec = C_EDGE;
        float th = 2;
        if (both_visited) {
          ec = C_PATH;
          th = 5;
        } else if (one_current && !both_visited) {
          ec = C_CURR;
          th = 3;
        }
        DrawLineEx(POS[i], POS[j], th, ec);

        /* Weight label with background */
        Vector2 mid = {(POS[i].x + POS[j].x) / 2, (POS[i].y + POS[j].y) / 2};
        char wt[16];
        snprintf(wt, sizeof(wt), "%d", graph.dist[i][j]);
        int tw = MeasureText(wt, 20);
        DrawRectangle((int)mid.x - tw / 2 - 6, (int)mid.y - 14, tw + 12, 26,
                      C_BG);
        DrawRectangleLines((int)mid.x - tw / 2 - 6, (int)mid.y - 14, tw + 12,
                           26, both_visited ? C_PATH : C_EDGE);
        DrawText(wt, (int)mid.x - tw / 2, (int)mid.y - 10, 20,
                 both_visited ? C_PATH : C_WHITE);
      }
    }
  }

  /* Draw nodes with distance labels inside */
  for (int i = 0; i < graph.num_cities; i++) {
    Color nc = C_NODE;
    Color ring = C_WHITE;
    if (dijk_visited[i]) {
      nc = C_VISIT;
      ring = C_VISIT;
    }
    if (i == dijk_current && !dijk_visited[i]) {
      nc = C_CURR;
      ring = C_GOLD;
    }

    /* Outer glow for current node */
    if (i == dijk_current && !dijk_visited[i]) {
      DrawCircleV(POS[i], 48, (Color){255, 200, 0, 60});
    }

    DrawCircleV(POS[i], 40, nc);
    DrawCircleLines((int)POS[i].x, (int)POS[i].y, 40, ring);
    DrawCircleLines((int)POS[i].x, (int)POS[i].y, 42, ring);

    /* City name above node */
    int nw = MeasureText(graph.cities[i].name, 18);
    DrawText(graph.cities[i].name, (int)(POS[i].x - nw / 2),
             (int)(POS[i].y - 62), 18, C_WHITE);

    /* Distance inside node */
    char dist_str[16];
    if (dijk_dist[i] == INF)
      snprintf(dist_str, sizeof(dist_str), "INF");
    else
      snprintf(dist_str, sizeof(dist_str), "%d", dijk_dist[i]);
    int dw = MeasureText(dist_str, 22);
    DrawText(dist_str, (int)(POS[i].x - dw / 2), (int)(POS[i].y - 11), 22,
             C_BG);

    /* Status indicator below */
    const char *status =
        dijk_visited[i] ? "DONE" : (i == dijk_current ? "CURR" : "");
    if (strlen(status) > 0) {
      int sw = MeasureText(status, 14);
      DrawText(status, (int)(POS[i].x - sw / 2), (int)(POS[i].y + 48), 14,
               dijk_visited[i] ? C_VISIT : C_GOLD);
    }
  }

  /* Info panel */
  DrawRectangle(CANVAS_W, 0, INFO_W, WINDOW_H - STORY_H, C_PANEL);

  /* Algorithm header with paradigm badge */
  DrawText("DIJKSTRA'S ALGORITHM", CANVAS_W + 20, 20, 28, C_CYAN);
  DrawRectangleRounded((Rectangle){CANVAS_W + 20, 55, 80, 24}, 0.3f, 4, C_CYAN);
  DrawText("GREEDY", CANVAS_W + 30, 59, 14, C_BG);
  DrawText("O(V^2)", CANVAS_W + 120, 58, 18, C_DIM);

  /* Progress bar */
  DrawText("Progress:", CANVAS_W + 20, 95, 18, C_DIM);
  DrawRectangle(CANVAS_W + 100, 95, 200, 20, C_CARD);
  DrawRectangle(CANVAS_W + 100, 95, (200 * dijk_step) / 6, 20, C_CYAN);
  DrawRectangleLines(CANVAS_W + 100, 95, 200, 20, C_EDGE);

  /* Distance table with visual indicators */
  DrawText("Distances:", CANVAS_W + 20, 125, 18, C_WHITE);
  DrawRectangle(CANVAS_W + 15, 148, INFO_W - 30, 195, C_CARD);

  int ty = 155;
  for (int i = 0; i < graph.num_cities; i++) {
    /* Status icon */
    if (dijk_visited[i]) {
      DrawCircle(CANVAS_W + 32, ty + 10, 6, C_VISIT);
    } else if (i == dijk_current) {
      DrawCircle(CANVAS_W + 32, ty + 10, 6, C_CURR);
    } else {
      DrawCircle(CANVAS_W + 32, ty + 10, 6, C_EDGE);
    }

    /* City name */
    DrawText(graph.cities[i].name, CANVAS_W + 48, ty, 16, C_WHITE);

    /* Distance value aligned right */
    char dist[32];
    if (dijk_dist[i] == INF)
      snprintf(dist, sizeof(dist), "INF");
    else
      snprintf(dist, sizeof(dist), "%d km", dijk_dist[i]);
    int dw = MeasureText(dist, 16);
    Color dc = C_DIM;
    if (dijk_visited[i])
      dc = C_VISIT;
    if (i == dijk_current && !dijk_visited[i])
      dc = C_GOLD;
    DrawText(dist, CANVAS_W + INFO_W - 45 - dw, ty, 16, dc);

    ty += 30;
  }

  /* Legend - moved down */
  int ly = 360;
  DrawText("LEGEND:", CANVAS_W + 20, ly, 14, C_DIM);
  DrawCircle(CANVAS_W + 30, ly + 22, 7, C_NODE);
  DrawText("Unvisited", CANVAS_W + 45, ly + 16, 12, C_WHITE);
  DrawCircle(CANVAS_W + 130, ly + 22, 7, C_CURR);
  DrawText("Current", CANVAS_W + 145, ly + 16, 12, C_WHITE);
  DrawCircle(CANVAS_W + 220, ly + 22, 7, C_VISIT);
  DrawText("Done", CANVAS_W + 235, ly + 16, 12, C_WHITE);

  /* Controls */
  DrawRectangle(CANVAS_W + 15, WINDOW_H - STORY_H - 70, INFO_W - 30, 55,
                C_CARD);
  DrawText("CONTROLS:", CANVAS_W + 25, WINDOW_H - STORY_H - 62, 14, C_CYAN);
  DrawText("SPACE=Step  A=Auto  R=Reset  ESC=Back", CANVAS_W + 25,
           WINDOW_H - STORY_H - 44, 13, C_DIM);
  if (auto_advance)
    DrawText("[AUTO]", CANVAS_W + 25, WINDOW_H - STORY_H - 26, 13, C_GOLD);

  draw_story_bar(DIJKSTRA_STORIES[(dijk_step < 7) ? dijk_step : 6]);
}

static void step_kruskal(void) {
  if (kruskal_step < NUM_EDGES) {
    if (kruskal_step == 0 || kruskal_step == 1 || kruskal_step == 2 ||
        kruskal_step == 4 || kruskal_step == 6)
      EDGES[kruskal_step].added = 1;
    else
      EDGES[kruskal_step].added = -1;
    kruskal_step++;
  }
}

static void draw_kruskal(void) {
  DrawRectangle(0, 0, WINDOW_W, WINDOW_H - STORY_H, C_BG);

  /* Title bar */
  DrawRectangle(0, 0, CANVAS_W, 50, C_PANEL);
  DrawText("MINIMUM SPANNING TREE", 20, 12, 28, C_MST_E);
  char step_info[64];
  snprintf(step_info, sizeof(step_info), "Edge %d/%d", kruskal_step, NUM_EDGES);
  DrawText(step_info, CANVAS_W - 130, 15, 22, C_GOLD);

  /* Draw edges with glow effect for current */
  for (int i = 0; i < NUM_EDGES; i++) {
    Edge *e = &EDGES[i];
    Color ec = C_EDGE;
    float th = 2;
    if (i < kruskal_step) {
      if (e->added == 1) {
        ec = C_MST_E;
        th = 6;
      } else {
        ec = C_REJECT;
        th = 2;
      }
    } else if (i == kruskal_step) {
      ec = C_CURR;
      th = 6;
      DrawLineEx(POS[e->u], POS[e->v], 14, (Color){255, 200, 50, 40});
    }
    DrawLineEx(POS[e->u], POS[e->v], th, ec);

    Vector2 mid = {(POS[e->u].x + POS[e->v].x) / 2,
                   (POS[e->u].y + POS[e->v].y) / 2};
    char wt[16];
    snprintf(wt, sizeof(wt), "%d", e->weight);
    int tw = MeasureText(wt, 18);
    Color border = (i < kruskal_step && e->added == 1)    ? C_MST_E
                   : (i < kruskal_step && e->added == -1) ? C_REJECT
                   : (i == kruskal_step)                  ? C_GOLD
                                                          : C_EDGE;
    DrawRectangle((int)mid.x - tw / 2 - 6, (int)mid.y - 12, tw + 12, 22, C_BG);
    DrawRectangleLines((int)mid.x - tw / 2 - 6, (int)mid.y - 12, tw + 12, 22,
                       border);
    DrawText(wt, (int)mid.x - tw / 2, (int)mid.y - 9, 18, border);
  }

  /* Count MST edges and check node connectivity */
  int mst_edges = 0;
  for (int i = 0; i < kruskal_step; i++)
    if (EDGES[i].added == 1)
      mst_edges++;

  for (int i = 0; i < 6; i++) {
    int connected = 0;
    for (int j = 0; j < kruskal_step; j++)
      if (EDGES[j].added == 1 && (EDGES[j].u == i || EDGES[j].v == i)) {
        connected = 1;
        break;
      }
    Color nc = connected ? C_MST_E : C_NODE;
    DrawCircleV(POS[i], 40, nc);
    DrawCircleLines((int)POS[i].x, (int)POS[i].y, 40, C_WHITE);

    int nw = MeasureText(graph.cities[i].name, 18);
    DrawText(graph.cities[i].name, (int)(POS[i].x - nw / 2),
             (int)(POS[i].y - 60), 18, C_WHITE);
    char num[4];
    snprintf(num, sizeof(num), "%d", i + 1);
    int numw = MeasureText(num, 24);
    DrawText(num, (int)(POS[i].x - numw / 2), (int)(POS[i].y - 12), 24, C_BG);
  }

  /* Info panel */
  DrawRectangle(CANVAS_W, 0, INFO_W, WINDOW_H - STORY_H, C_PANEL);
  DrawText("KRUSKAL'S ALGORITHM", CANVAS_W + 20, 20, 26, C_MST_E);
  DrawRectangleRounded((Rectangle){CANVAS_W + 20, 52, 80, 24}, 0.3f, 4,
                       C_MST_E);
  DrawText("GREEDY", CANVAS_W + 30, 56, 14, C_BG);
  DrawText("O(E log E)", CANVAS_W + 115, 55, 16, C_DIM);

  /* Progress bar */
  DrawText("Progress:", CANVAS_W + 20, 88, 16, C_DIM);
  DrawRectangle(CANVAS_W + 100, 88, 200, 18, C_CARD);
  DrawRectangle(CANVAS_W + 100, 88, (200 * kruskal_step) / NUM_EDGES, 18,
                C_MST_E);
  DrawRectangleLines(CANVAS_W + 100, 88, 200, 18, C_EDGE);

  /* Edge list */
  DrawText("Edges (by weight):", CANVAS_W + 20, 115, 16, C_WHITE);
  DrawRectangle(CANVAS_W + 15, 135, INFO_W - 30, 210, C_CARD);

  int ty = 142, total = 0;
  for (int i = 0; i < NUM_EDGES; i++) {
    Edge *e = &EDGES[i];
    Color status_col = C_EDGE;
    const char *status = "";
    if (i < kruskal_step) {
      if (e->added == 1) {
        status_col = C_MST_E;
        status = "ADD";
        total += e->weight;
      } else {
        status_col = C_REJECT;
        status = "SKIP";
      }
    } else if (i == kruskal_step) {
      status_col = C_GOLD;
      status = ">>";
    }
    DrawCircle(CANVAS_W + 28, ty + 8, 4, status_col);
    char line[64];
    snprintf(line, sizeof(line), "%s-%s: %d", graph.cities[e->u].name,
             graph.cities[e->v].name, e->weight);
    DrawText(line, CANVAS_W + 40, ty, 13,
             (i <= kruskal_step) ? status_col : C_DIM);
    if (strlen(status) > 0) {
      int sw = MeasureText(status, 11);
      DrawText(status, CANVAS_W + INFO_W - 40 - sw, ty + 1, 11, status_col);
    }
    ty += 25;
  }

  /* MST Statistics */
  DrawRectangle(CANVAS_W + 15, 355, INFO_W - 30, 55, C_CARD);
  DrawText("MST STATS:", CANVAS_W + 25, 362, 14, C_GOLD);
  char stat[64];
  snprintf(stat, sizeof(stat), "Edges: %d/5  Cost: %d km", mst_edges, total);
  DrawText(stat, CANVAS_W + 35, 382, 16, C_WHITE);

  /* Legend */
  DrawText("LEGEND:", CANVAS_W + 20, 420, 12, C_DIM);
  DrawCircle(CANVAS_W + 30, 440, 5, C_MST_E);
  DrawText("Added", CANVAS_W + 42, 434, 11, C_WHITE);
  DrawCircle(CANVAS_W + 100, 440, 5, C_REJECT);
  DrawText("Rejected", CANVAS_W + 112, 434, 11, C_WHITE);
  DrawCircle(CANVAS_W + 185, 440, 5, C_GOLD);
  DrawText("Current", CANVAS_W + 197, 434, 11, C_WHITE);

  /* Controls */
  DrawRectangle(CANVAS_W + 15, WINDOW_H - STORY_H - 65, INFO_W - 30, 50,
                C_CARD);
  DrawText("CONTROLS:", CANVAS_W + 25, WINDOW_H - STORY_H - 58, 12, C_CYAN);
  DrawText("SPACE=Step  A=Auto  R=Reset  ESC=Back", CANVAS_W + 25,
           WINDOW_H - STORY_H - 42, 12, C_DIM);
  if (auto_advance)
    DrawText("[AUTO]", CANVAS_W + 25, WINDOW_H - STORY_H - 26, 12, C_GOLD);

  draw_story_bar(KRUSKAL_STORIES[(kruskal_step < 8) ? kruskal_step : 7]);
}

/* TSP */
static int tsp_best_cost, tsp_best_path[MAX_CITIES];
static long tsp_nodes_explored;
static int tsp_visited[MAX_CITIES], tsp_path[MAX_CITIES];

static void tsp_bf_recurse(Graph *g, int depth, int curr, int cost, int n) {
  tsp_nodes_explored++;
  if (depth == n) {
    int total = cost + g->dist[curr][tsp_path[0]];
    if (total < tsp_best_cost) {
      tsp_best_cost = total;
      for (int i = 0; i < n; i++)
        tsp_best_path[i] = tsp_path[i];
    }
    return;
  }
  for (int v = 0; v < n; v++) {
    if (!tsp_visited[v] && g->dist[curr][v] != INF) {
      tsp_visited[v] = 1;
      tsp_path[depth] = v;
      tsp_bf_recurse(g, depth + 1, v, cost + g->dist[curr][v], n);
      tsp_visited[v] = 0;
    }
  }
}

static int tsp_lower_bound(Graph *g, int depth, int n) {
  int bound = 0;
  for (int i = 0; i < depth - 1; i++)
    bound += g->dist[tsp_path[i]][tsp_path[i + 1]];
  for (int v = 0; v < n; v++) {
    if (tsp_visited[v])
      continue;
    int min1 = INF;
    for (int u = 0; u < n; u++)
      if (u != v && g->dist[v][u] != INF && g->dist[v][u] < min1)
        min1 = g->dist[v][u];
    if (min1 != INF)
      bound += min1;
  }
  return bound;
}

static void tsp_bb_recurse(Graph *g, int depth, int curr, int cost, int n) {
  tsp_nodes_explored++;
  if (depth == n) {
    int total = cost + g->dist[curr][tsp_path[0]];
    if (total < tsp_best_cost) {
      tsp_best_cost = total;
      for (int i = 0; i < n; i++)
        tsp_best_path[i] = tsp_path[i];
    }
    return;
  }
  for (int v = 0; v < n; v++) {
    if (tsp_visited[v] || g->dist[curr][v] == INF)
      continue;
    tsp_visited[v] = 1;
    tsp_path[depth] = v;
    int new_cost = cost + g->dist[curr][v];
    int bound = tsp_lower_bound(g, depth + 1, n);
    if (new_cost + bound < tsp_best_cost)
      tsp_bb_recurse(g, depth + 1, v, new_cost, n);
    else
      tsp_bb_pruned++;
    tsp_visited[v] = 0;
  }
}

static void compute_tsp(void) {
  if (tsp_computed)
    return;
  int n = graph.num_cities;
  if (n == 0)
    return;
  tsp_best_cost = INF;
  tsp_nodes_explored = 0;
  for (int i = 0; i < n; i++)
    tsp_visited[i] = 0;
  tsp_visited[0] = 1;
  tsp_path[0] = 0;
  tsp_bf_recurse(&graph, 1, 0, 0, n);
  tsp_bf_nodes = (int)tsp_nodes_explored;
  tsp_bf_cost = tsp_best_cost;
  for (int i = 0; i < n; i++)
    tsp_bf_tour[i] = tsp_best_path[i];

  tsp_best_cost = INF;
  tsp_nodes_explored = 0;
  tsp_bb_pruned = 0;
  for (int i = 0; i < n; i++)
    tsp_visited[i] = 0;
  tsp_visited[0] = 1;
  tsp_path[0] = 0;
  tsp_bb_recurse(&graph, 1, 0, 0, n);
  tsp_bb_nodes = (int)tsp_nodes_explored;
  tsp_bb_cost = tsp_best_cost;
  for (int i = 0; i < n; i++)
    tsp_bb_tour[i] = tsp_best_path[i];
  tsp_computed = 1;
}

static void draw_tsp(void) {
  compute_tsp();
  DrawRectangle(0, 0, WINDOW_W, WINDOW_H - STORY_H, C_BG);

  /* Title bar */
  DrawRectangle(0, 0, CANVAS_W, 50, C_PANEL);
  DrawText("TRAVELING SALESMAN PROBLEM", 20, 12, 28, C_ORANGE);
  const char *mode_str = (tsp_mode == 0) ? "Brute Force" : "Branch & Bound";
  int mw = MeasureText(mode_str, 20);
  DrawText(mode_str, CANVAS_W - mw - 20, 15, 20, (tsp_mode == 0) ? C_BF : C_BB);

  int n = graph.num_cities;
  int *tour = (tsp_mode == 0) ? tsp_bf_tour : tsp_bb_tour;
  Color tc = (tsp_mode == 0) ? C_BF : C_BB;

  /* Draw all edges faintly */
  for (int i = 0; i < n; i++)
    for (int j = i + 1; j < n; j++)
      if (graph.dist[i][j] < INF)
        DrawLineEx(POS[i], POS[j], 1, (Color){70, 80, 100, 100});

  /* Draw tour edges with arrows and distance labels */
  for (int i = 0; i < n; i++) {
    int from = tour[i], to = tour[(i + 1) % n];

    /* Glow effect */
    DrawLineEx(POS[from], POS[to], 10, (Color){tc.r, tc.g, tc.b, 40});
    DrawLineEx(POS[from], POS[to], 6, tc);

    /* Arrow at midpoint */
    Vector2 mid = {(POS[from].x + POS[to].x) / 2,
                   (POS[from].y + POS[to].y) / 2};
    float dx = POS[to].x - POS[from].x, dy = POS[to].y - POS[from].y;
    float len = sqrtf(dx * dx + dy * dy);
    if (len > 0) {
      dx /= len;
      dy /= len;
      Vector2 tip = {mid.x + dx * 14, mid.y + dy * 14};
      Vector2 l = {mid.x - dx * 7 - dy * 7, mid.y - dy * 7 + dx * 7};
      Vector2 r = {mid.x - dx * 7 + dy * 7, mid.y - dy * 7 - dx * 7};
      DrawTriangle(tip, l, r, tc);
    }

    /* Distance label */
    char dist[16];
    snprintf(dist, sizeof(dist), "%d", graph.dist[from][to]);
    int dw = MeasureText(dist, 16);
    Vector2 label_pos = {mid.x + dy * 20, mid.y - dx * 20};
    DrawRectangle((int)label_pos.x - dw / 2 - 4, (int)label_pos.y - 10, dw + 8,
                  18, C_BG);
    DrawText(dist, (int)label_pos.x - dw / 2, (int)label_pos.y - 8, 16, tc);
  }

  /* Draw nodes with visit order */
  for (int i = 0; i < n; i++) {
    int idx = tour[i];

    /* Node circle with order number */
    DrawCircleV(POS[idx], 42, tc);
    DrawCircleV(POS[idx], 36, C_NODE);
    DrawCircleLines((int)POS[idx].x, (int)POS[idx].y, 42, tc);

    /* Visit order inside */
    char ord[8];
    snprintf(ord, sizeof(ord), "%d", i + 1);
    int ow = MeasureText(ord, 28);
    DrawText(ord, (int)(POS[idx].x - ow / 2), (int)(POS[idx].y - 14), 28,
             C_WHITE);

    /* City name below */
    int nw = MeasureText(graph.cities[idx].name, 16);
    DrawText(graph.cities[idx].name, (int)(POS[idx].x - nw / 2),
             (int)(POS[idx].y + 50), 16, C_WHITE);

    /* Start indicator */
    if (i == 0) {
      DrawText("START", (int)(POS[idx].x - 25), (int)(POS[idx].y - 70), 14,
               C_GOLD);
    }
  }

  /* Info panel */
  DrawRectangle(CANVAS_W, 0, INFO_W, WINDOW_H - STORY_H, C_PANEL);
  DrawText("TSP COMPARISON", CANVAS_W + 20, 20, 28, C_ORANGE);
  DrawRectangleRounded((Rectangle){CANVAS_W + 20, 52, 110, 24}, 0.3f, 4,
                       C_ORANGE);
  DrawText("BRANCH&BOUND", CANVAS_W + 27, 56, 12, C_BG);
  DrawText("NP-Hard", CANVAS_W + 145, 55, 16, C_DIM);

  /* Algorithm selection buttons */
  DrawText("Select Algorithm:", CANVAS_W + 20, 90, 18, C_WHITE);
  Rectangle bf_btn = {CANVAS_W + 20, 115, 150, 45};
  Rectangle bb_btn = {CANVAS_W + 180, 115, 150, 45};
  draw_button(bf_btn, "1-Brute Force", tsp_mode == 0, C_BF);
  draw_button(bb_btn, "2-Branch&Bound", tsp_mode == 1, C_BB);

  /* Results card */
  int sy = 175;
  DrawRectangle(CANVAS_W + 15, sy, INFO_W - 30, 150, C_CARD);

  char line[128];
  if (tsp_mode == 0) {
    DrawText("BRUTE FORCE:", CANVAS_W + 25, sy + 10, 20, C_BF);
    DrawText("Explores ALL permutations", CANVAS_W + 35, sy + 35, 14, C_DIM);
    snprintf(line, sizeof(line), "Nodes: %d", tsp_bf_nodes);
    DrawText(line, CANVAS_W + 35, sy + 60, 18, C_WHITE);
    DrawText("Pruning: 0%", CANVAS_W + 35, sy + 85, 18, C_WHITE);
    snprintf(line, sizeof(line), "Tour Cost: %d km", tsp_bf_cost);
    DrawText(line, CANVAS_W + 35, sy + 115, 20, C_GOLD);
  } else {
    DrawText("BRANCH & BOUND:", CANVAS_W + 25, sy + 10, 20, C_BB);
    DrawText("Prunes unpromising branches", CANVAS_W + 35, sy + 35, 14, C_DIM);
    snprintf(line, sizeof(line), "Nodes: %d", tsp_bb_nodes);
    DrawText(line, CANVAS_W + 35, sy + 60, 18, C_WHITE);
    double pct = (tsp_bf_nodes > 0)
                     ? (100.0 * (tsp_bf_nodes - tsp_bb_nodes) / tsp_bf_nodes)
                     : 0;
    snprintf(line, sizeof(line), "Pruning: %.0f%%", pct);
    DrawText(line, CANVAS_W + 35, sy + 85, 18, C_VISIT);
    snprintf(line, sizeof(line), "Tour Cost: %d km", tsp_bb_cost);
    DrawText(line, CANVAS_W + 35, sy + 115, 20, C_GOLD);
  }

  /* Comparison section */
  int cy = sy + 165;
  DrawRectangle(CANVAS_W + 15, cy, INFO_W - 30, 130, C_CARD);
  DrawText("EFFICIENCY COMPARISON:", CANVAS_W + 25, cy + 10, 18, C_CYAN);

  /* Visual bar comparison */
  int bar_y = cy + 40;
  DrawText("BF:", CANVAS_W + 25, bar_y, 14, C_BF);
  int bf_bar_w = 200;
  DrawRectangle(CANVAS_W + 55, bar_y, bf_bar_w, 16, C_BF);
  snprintf(line, sizeof(line), "%d", tsp_bf_nodes);
  DrawText(line, CANVAS_W + 260, bar_y, 14, C_WHITE);

  DrawText("BB:", CANVAS_W + 25, bar_y + 25, 14, C_BB);
  int bb_bar_w = (tsp_bf_nodes > 0) ? (200 * tsp_bb_nodes / tsp_bf_nodes) : 0;
  DrawRectangle(CANVAS_W + 55, bar_y + 25, bb_bar_w, 16, C_BB);
  snprintf(line, sizeof(line), "%d", tsp_bb_nodes);
  DrawText(line, CANVAS_W + 260, bar_y + 25, 14, C_WHITE);

  snprintf(line, sizeof(line), "Pruned: %ld branches", tsp_bb_pruned);
  DrawText(line, CANVAS_W + 35, bar_y + 55, 16, C_DIM);
  snprintf(line, sizeof(line), "Work saved: %d nodes!",
           tsp_bf_nodes - tsp_bb_nodes);
  DrawText(line, CANVAS_W + 35, bar_y + 78, 18, C_VISIT);

  /* Controls */
  DrawRectangle(CANVAS_W + 15, WINDOW_H - STORY_H - 70, INFO_W - 30, 55,
                C_CARD);
  DrawText("CONTROLS:", CANVAS_W + 25, WINDOW_H - STORY_H - 62, 14, C_CYAN);
  DrawText("Press 1/2 to switch | ESC=Back", CANVAS_W + 25,
           WINDOW_H - STORY_H - 42, 14, C_DIM);

  char story[256];
  if (tsp_mode == 0)
    snprintf(
        story, sizeof(story),
        "BRUTE FORCE: Checks ALL %d! = %d permutations. Guarantees optimal "
        "but O(n!) complexity - impractical for large n!",
        n, tsp_bf_nodes);
  else
    snprintf(story, sizeof(story),
             "BRANCH & BOUND: Uses lower bound to prune branches. Same optimal "
             "%d km tour with only %d nodes - %.0f%% less work!",
             tsp_bb_cost, tsp_bb_nodes,
             (tsp_bf_nodes > 0)
                 ? (100.0 * (tsp_bf_nodes - tsp_bb_nodes) / tsp_bf_nodes)
                 : 0.0);
  draw_story_bar(story);
}

/* KNAPSACK */
static void compute_knapsack(void) {
  if (knapsack_computed)
    return;
  int n = num_items, cap = knapsack_capacity;
  if (n == 0)
    return;

  static int dp[MAX_ITEMS + 1][MAX_CAPACITY + 1];
  for (int i = 0; i <= n; i++) {
    for (int w = 0; w <= cap; w++) {
      if (i == 0 || w == 0)
        dp[i][w] = 0;
      else if (items[i - 1].weight <= w) {
        int take = items[i - 1].value + dp[i - 1][w - items[i - 1].weight];
        int skip = dp[i - 1][w];
        dp[i][w] = (take > skip) ? take : skip;
      } else
        dp[i][w] = dp[i - 1][w];
    }
  }

  for (int i = 0; i < n; i++)
    knapsack_dp_selected[i] = 0;
  int w = cap;
  for (int i = n; i > 0; i--) {
    if (dp[i][w] != dp[i - 1][w]) {
      knapsack_dp_selected[i - 1] = 1;
      w -= items[i - 1].weight;
    }
  }
  knapsack_dp_total_weight = 0;
  knapsack_dp_total_value = 0;
  for (int i = 0; i < n; i++) {
    if (knapsack_dp_selected[i]) {
      knapsack_dp_total_weight += items[i].weight;
      knapsack_dp_total_value += items[i].value;
    }
  }

  for (int i = 0; i < n; i++)
    knapsack_sorted[i] = items[i];
  for (int i = 1; i < n; i++) {
    CargoItem key = knapsack_sorted[i];
    double kr = (double)key.value / key.weight;
    int j = i - 1;
    while (j >= 0 &&
           (double)knapsack_sorted[j].value / knapsack_sorted[j].weight < kr) {
      knapsack_sorted[j + 1] = knapsack_sorted[j];
      j--;
    }
    knapsack_sorted[j + 1] = key;
  }

  for (int i = 0; i < n; i++)
    knapsack_greedy_fraction[i] = 0.0;
  int rem = cap;
  knapsack_greedy_total_weight = 0;
  knapsack_greedy_total_value = 0.0;
  for (int i = 0; i < n && rem > 0; i++) {
    if (knapsack_sorted[i].weight <= rem) {
      knapsack_greedy_fraction[i] = 1.0;
      rem -= knapsack_sorted[i].weight;
      knapsack_greedy_total_weight += knapsack_sorted[i].weight;
      knapsack_greedy_total_value += knapsack_sorted[i].value;
    } else {
      knapsack_greedy_fraction[i] = (double)rem / knapsack_sorted[i].weight;
      knapsack_greedy_total_value +=
          knapsack_greedy_fraction[i] * knapsack_sorted[i].value;
      knapsack_greedy_total_weight += rem;
      rem = 0;
    }
  }
  knapsack_computed = 1;
}

static void draw_knapsack(void) {
  compute_knapsack();
  DrawRectangle(0, 0, WINDOW_W, WINDOW_H - STORY_H, C_BG);

  /* Title bar */
  DrawRectangle(0, 0, CANVAS_W, 50, C_PANEL);
  DrawText("0/1 KNAPSACK PROBLEM", 20, 12, 28, C_GOLD);
  const char *mode_str =
      (knapsack_mode == 0) ? "DP Optimal" : "Greedy Fractional";
  int mw = MeasureText(mode_str, 18);
  DrawText(mode_str, CANVAS_W - mw - 20, 16, 18,
           (knapsack_mode == 0) ? C_CYAN : C_ORANGE);

  /* Capacity indicator */
  DrawRectangle(20, 60, CANVAS_W - 40, 35, C_CARD);
  DrawText("TRUCK CAPACITY:", 30, 68, 16, C_DIM);
  int cap_bar_w = CANVAS_W - 200;
  DrawRectangle(160, 68, cap_bar_w, 20, C_EDGE);
  int used_w = (knapsack_mode == 0) ? knapsack_dp_total_weight
                                    : knapsack_greedy_total_weight;
  int fill_w = (cap_bar_w * used_w) / knapsack_capacity;
  Color fill_col = (used_w <= knapsack_capacity) ? C_VISIT : C_REJECT;
  DrawRectangle(160, 68, fill_w, 20, fill_col);
  DrawRectangleLines(160, 68, cap_bar_w, 20, C_WHITE);
  char cap_text[32];
  snprintf(cap_text, sizeof(cap_text), "%d/%d kg", used_w, knapsack_capacity);
  DrawText(cap_text, CANVAS_W - 100, 68, 18, C_WHITE);

  int n = num_items;
  int max_val = 1;
  for (int i = 0; i < n; i++)
    if (items[i].value > max_val)
      max_val = items[i].value;

  /* Draw cargo items as boxes */
  int bar_y = 110, bar_x = 50, bar_sp = 90, max_h = 350;
  for (int i = 0; i < n && i < 8; i++) {
    int bar_h = (items[i].value * max_h) / max_val;
    if (bar_h < 40)
      bar_h = 40;
    Rectangle bar = {(float)(bar_x + i * bar_sp),
                     (float)(bar_y + max_h - bar_h), 75, (float)bar_h};

    Color bc = C_NODE;
    Color border = C_EDGE;
    double frac = 0;
    int selected = 0;

    if (knapsack_mode == 0) {
      if (knapsack_dp_selected[i]) {
        bc = C_CYAN;
        border = C_CYAN;
        selected = 1;
      }
    } else {
      for (int j = 0; j < n; j++) {
        if (strcmp(knapsack_sorted[j].name, items[i].name) == 0) {
          frac = knapsack_greedy_fraction[j];
          if (frac >= 1.0) {
            bc = C_VISIT;
            border = C_VISIT;
            selected = 1;
          } else if (frac > 0) {
            bc = C_ORANGE;
            border = C_ORANGE;
            selected = 1;
          }
          break;
        }
      }
    }

    /* Draw box with selection indicator */
    if (selected) {
      DrawRectangleRec(
          (Rectangle){bar.x - 3, bar.y - 3, bar.width + 6, bar.height + 6},
          (Color){border.r, border.g, border.b, 60});
    }
    DrawRectangleRec(bar, bc);
    DrawRectangleLinesEx(bar, 2, border);

    /* Partial fill for fractional */
    if (knapsack_mode == 1 && frac > 0 && frac < 1.0) {
      int partial_h = (int)(bar.height * frac);
      DrawRectangle((int)bar.x, (int)(bar.y + bar.height - partial_h),
                    (int)bar.width, partial_h, C_VISIT);
      DrawLine((int)bar.x, (int)(bar.y + bar.height - partial_h),
               (int)(bar.x + bar.width), (int)(bar.y + bar.height - partial_h),
               C_WHITE);
    }

    /* Item name */
    char nm[12];
    strncpy(nm, items[i].name, 7);
    nm[7] = '\0';
    int nw = MeasureText(nm, 14);
    DrawText(nm, (int)(bar.x + (75 - nw) / 2), bar_y + max_h + 8, 14, C_WHITE);

    /* Weight and value */
    char wv[32];
    snprintf(wv, sizeof(wv), "%dkg", items[i].weight);
    int ww = MeasureText(wv, 13);
    DrawText(wv, (int)(bar.x + (75 - ww) / 2), bar_y + max_h + 26, 13, C_DIM);

    snprintf(wv, sizeof(wv), "Rs%dk", items[i].value);
    int vw = MeasureText(wv, 14);
    DrawText(wv, (int)(bar.x + (75 - vw) / 2), bar_y + max_h + 44, 14, C_GOLD);

    /* Value/weight ratio */
    double ratio = (double)items[i].value / items[i].weight;
    snprintf(wv, sizeof(wv), "r=%.1f", ratio);
    int rw = MeasureText(wv, 12);
    DrawText(wv, (int)(bar.x + (75 - rw) / 2), bar_y + max_h + 62, 12, C_DIM);
  }

  /* Info panel */
  DrawRectangle(CANVAS_W, 0, INFO_W, WINDOW_H - STORY_H, C_PANEL);
  DrawText("KNAPSACK PROBLEM", CANVAS_W + 20, 20, 26, C_GOLD);
  DrawRectangleRounded((Rectangle){CANVAS_W + 20, 50, 100, 22}, 0.3f, 4,
                       C_GOLD);
  DrawText("DYNAMIC PROG", CANVAS_W + 27, 54, 11, C_BG);
  DrawText("O(n*W)", CANVAS_W + 135, 52, 16, C_DIM);

  /* Algorithm selection */
  DrawText("Select Approach:", CANVAS_W + 20, 85, 16, C_WHITE);
  Rectangle dp_btn = {CANVAS_W + 20, 105, 150, 40};
  Rectangle gr_btn = {CANVAS_W + 180, 105, 150, 40};
  draw_button(dp_btn, "1-0/1 DP", knapsack_mode == 0, C_CYAN);
  draw_button(gr_btn, "2-Greedy", knapsack_mode == 1, C_ORANGE);

  /* Results card */
  int sy = 160;
  DrawRectangle(CANVAS_W + 15, sy, INFO_W - 30, 200, C_CARD);

  char line[128];
  if (knapsack_mode == 0) {
    DrawText("0/1 DP SOLUTION:", CANVAS_W + 25, sy + 10, 18, C_CYAN);
    DrawText("(Optimal - considers all combos)", CANVAS_W + 35, sy + 32, 12,
             C_DIM);
    DrawText("Selected items:", CANVAS_W + 35, sy + 55, 16, C_WHITE);
    int y = sy + 75;
    for (int i = 0; i < n; i++) {
      if (knapsack_dp_selected[i]) {
        snprintf(line, sizeof(line), "+ %s (%dkg, Rs%dk)", items[i].name,
                 items[i].weight, items[i].value);
        DrawText(line, CANVAS_W + 45, y, 14, C_WHITE);
        y += 22;
        if (y > sy + 150)
          break;
      }
    }
    y = sy + 160;
    snprintf(line, sizeof(line), "Weight: %d/%d kg", knapsack_dp_total_weight,
             knapsack_capacity);
    DrawText(line, CANVAS_W + 35, y, 16, C_WHITE);
    snprintf(line, sizeof(line), "Value: Rs%dk", knapsack_dp_total_value);
    DrawText(line, CANVAS_W + 200, y, 16, C_GOLD);
    DrawText("OPTIMAL", CANVAS_W + 35, y + 22, 14, C_VISIT);
  } else {
    DrawText("GREEDY FRACTIONAL:", CANVAS_W + 25, sy + 10, 18, C_ORANGE);
    DrawText("(Sorts by value/weight ratio)", CANVAS_W + 35, sy + 32, 12,
             C_DIM);
    DrawText("Items taken:", CANVAS_W + 35, sy + 55, 16, C_WHITE);
    int y = sy + 75, has_frac = 0;
    for (int i = 0; i < n && i < 4; i++) {
      if (knapsack_greedy_fraction[i] > 0) {
        double ratio =
            (double)knapsack_sorted[i].value / knapsack_sorted[i].weight;
        if (knapsack_greedy_fraction[i] >= 1.0) {
          snprintf(line, sizeof(line), "+ %s (100%%) r=%.1f",
                   knapsack_sorted[i].name, ratio);
          DrawText(line, CANVAS_W + 45, y, 14, C_VISIT);
        } else {
          snprintf(line, sizeof(line), "+ %s (%.0f%%) r=%.1f",
                   knapsack_sorted[i].name, knapsack_greedy_fraction[i] * 100,
                   ratio);
          DrawText(line, CANVAS_W + 45, y, 14, C_ORANGE);
          has_frac = 1;
        }
        y += 22;
      }
    }
    y = sy + 160;
    snprintf(line, sizeof(line), "Weight: %d/%d kg",
             knapsack_greedy_total_weight, knapsack_capacity);
    DrawText(line, CANVAS_W + 35, y, 16, C_WHITE);
    snprintf(line, sizeof(line), "Value: Rs%.0fk", knapsack_greedy_total_value);
    DrawText(line, CANVAS_W + 200, y, 16, C_GOLD);
    if (has_frac) {
      DrawText("INVALID for 0/1!", CANVAS_W + 35, y + 22, 14, C_REJECT);
    } else {
      DrawText("Valid", CANVAS_W + 35, y + 22, 14, C_VISIT);
    }
  }

  /* Comparison section */
  int cy = sy + 215;
  DrawRectangle(CANVAS_W + 15, cy, INFO_W - 30, 100, C_CARD);
  DrawText("VALUE COMPARISON:", CANVAS_W + 25, cy + 10, 16, C_CYAN);

  /* Visual bar comparison */
  int max_v = (knapsack_greedy_total_value > knapsack_dp_total_value)
                  ? (int)knapsack_greedy_total_value
                  : knapsack_dp_total_value;
  int bar_max = 180;

  DrawText("DP:", CANVAS_W + 25, cy + 35, 14, C_CYAN);
  int dp_w = (max_v > 0) ? (bar_max * knapsack_dp_total_value / max_v) : 0;
  DrawRectangle(CANVAS_W + 55, cy + 35, dp_w, 14, C_CYAN);
  snprintf(line, sizeof(line), "Rs%dk", knapsack_dp_total_value);
  DrawText(line, CANVAS_W + 250, cy + 35, 14, C_WHITE);

  DrawText("GR:", CANVAS_W + 25, cy + 55, 14, C_ORANGE);
  int gr_w =
      (max_v > 0) ? (bar_max * (int)knapsack_greedy_total_value / max_v) : 0;
  DrawRectangle(CANVAS_W + 55, cy + 55, gr_w, 14, C_ORANGE);
  snprintf(line, sizeof(line), "Rs%.0fk", knapsack_greedy_total_value);
  DrawText(line, CANVAS_W + 250, cy + 55, 14, C_WHITE);

  if (knapsack_greedy_total_value > knapsack_dp_total_value) {
    DrawText("Greedy higher but may be INVALID!", CANVAS_W + 35, cy + 78, 12,
             C_REJECT);
  } else {
    DrawText("DP gives true optimal!", CANVAS_W + 35, cy + 78, 12, C_VISIT);
  }

  /* Controls */
  DrawRectangle(CANVAS_W + 15, WINDOW_H - STORY_H - 65, INFO_W - 30, 50,
                C_CARD);
  DrawText("CONTROLS:", CANVAS_W + 25, WINDOW_H - STORY_H - 58, 14, C_CYAN);
  DrawText("Press 1/2 to switch | ESC=Back", CANVAS_W + 25,
           WINDOW_H - STORY_H - 38, 14, C_DIM);

  char story[256];
  if (knapsack_mode == 0)
    snprintf(story, sizeof(story),
             "0/1 DP: Builds table of optimal values for each capacity. "
             "Result: %dkg cargo worth Rs%dk - GUARANTEED OPTIMAL!",
             knapsack_dp_total_weight, knapsack_dp_total_value);
  else
    snprintf(story, sizeof(story),
             "GREEDY: Takes items by best value/weight ratio. Gets Rs%.0fk but "
             "may SPLIT items - violates 0/1 constraint!",
             knapsack_greedy_total_value);
  draw_story_bar(story);
}

void run_visualizer(void) {
  InitWindow(WINDOW_W, WINDOW_H, "Algorithm Visualizer - DAA Project");
  SetTargetFPS(60);
  SetExitKey(0);

  graph_load_sample(&graph);
  knapsack_load_sample(items, &num_items);
  reset_dijkstra();
  reset_kruskal();

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    if (auto_advance) {
      auto_timer += dt;
      if (auto_timer >= AUTO_DELAY) {
        auto_timer = 0.0f;
        if (current_screen == SCREEN_DIJKSTRA && dijk_step < 7)
          step_dijkstra();
        else if (current_screen == SCREEN_KRUSKAL && kruskal_step < NUM_EDGES)
          step_kruskal();
      }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
      if (current_screen == SCREEN_MENU)
        break;
      else if (current_screen == SCREEN_DIJKSTRA) {
        current_screen = SCREEN_DIJKSTRA_INTRO;
        auto_advance = 0;
        reset_dijkstra();
      } else if (current_screen == SCREEN_KRUSKAL) {
        current_screen = SCREEN_KRUSKAL_INTRO;
        auto_advance = 0;
        reset_kruskal();
      } else if (current_screen == SCREEN_TSP) {
        current_screen = SCREEN_TSP_INTRO;
        tsp_computed = 0;
      } else if (current_screen == SCREEN_KNAPSACK) {
        current_screen = SCREEN_KNAPSACK_INTRO;
        knapsack_computed = 0;
      } else
        current_screen = SCREEN_MENU;
    }

    if (current_screen == SCREEN_MENU) {
      if (IsKeyPressed(KEY_ONE))
        current_screen = SCREEN_DIJKSTRA_INTRO;
      if (IsKeyPressed(KEY_TWO))
        current_screen = SCREEN_KRUSKAL_INTRO;
      if (IsKeyPressed(KEY_THREE))
        current_screen = SCREEN_TSP_INTRO;
      if (IsKeyPressed(KEY_FOUR))
        current_screen = SCREEN_KNAPSACK_INTRO;
    } else if (current_screen == SCREEN_DIJKSTRA_INTRO ||
               current_screen == SCREEN_KRUSKAL_INTRO ||
               current_screen == SCREEN_TSP_INTRO ||
               current_screen == SCREEN_KNAPSACK_INTRO) {
      if (IsKeyPressed(KEY_ENTER)) {
        if (current_screen == SCREEN_DIJKSTRA_INTRO) {
          current_screen = SCREEN_DIJKSTRA;
          reset_dijkstra();
        } else if (current_screen == SCREEN_KRUSKAL_INTRO) {
          current_screen = SCREEN_KRUSKAL;
          reset_kruskal();
        } else if (current_screen == SCREEN_TSP_INTRO) {
          current_screen = SCREEN_TSP;
          tsp_computed = 0;
        } else if (current_screen == SCREEN_KNAPSACK_INTRO) {
          current_screen = SCREEN_KNAPSACK;
          knapsack_computed = 0;
        }
      }
    } else if (current_screen == SCREEN_DIJKSTRA) {
      if (IsKeyPressed(KEY_SPACE) && dijk_step < 7)
        step_dijkstra();
      if (IsKeyPressed(KEY_A)) {
        auto_advance = !auto_advance;
        auto_timer = 0.0f;
      }
      if (IsKeyPressed(KEY_R)) {
        reset_dijkstra();
        auto_advance = 0;
      }
    } else if (current_screen == SCREEN_KRUSKAL) {
      if (IsKeyPressed(KEY_SPACE) && kruskal_step < NUM_EDGES)
        step_kruskal();
      if (IsKeyPressed(KEY_A)) {
        auto_advance = !auto_advance;
        auto_timer = 0.0f;
      }
      if (IsKeyPressed(KEY_R)) {
        reset_kruskal();
        auto_advance = 0;
      }
    } else if (current_screen == SCREEN_TSP) {
      if (IsKeyPressed(KEY_ONE))
        tsp_mode = 0;
      if (IsKeyPressed(KEY_TWO))
        tsp_mode = 1;
    } else if (current_screen == SCREEN_KNAPSACK) {
      if (IsKeyPressed(KEY_ONE))
        knapsack_mode = 0;
      if (IsKeyPressed(KEY_TWO))
        knapsack_mode = 1;
    }

    BeginDrawing();
    switch (current_screen) {
    case SCREEN_MENU:
      draw_menu();
      break;
    case SCREEN_DIJKSTRA_INTRO:
      draw_intro("DIJKSTRA'S SHORTEST PATH",
                 "You manage deliveries from Bangalore to 5 cities. Each route "
                 "has a distance.\nWhat's the CHEAPEST way to reach each city?",
                 "Dijkstra's algorithm locks in cities one by one, always "
                 "choosing the closest.\nGuarantees shortest path in O(V^2) "
                 "time!",
                 C_CYAN);
      break;
    case SCREEN_DIJKSTRA:
      draw_dijkstra();
      break;
    case SCREEN_KRUSKAL_INTRO:
      draw_intro("KRUSKAL'S MINIMUM SPANNING TREE",
                 "You need to build roads connecting all 6 cities. Each road "
                 "has a cost.\nWhat's the CHEAPEST network that connects "
                 "everything?",
                 "Kruskal sorts roads by cost and adds them one by one, "
                 "skipping cycles.\nResult: cheapest connected network!",
                 C_MST_E);
      break;
    case SCREEN_KRUSKAL:
      draw_kruskal();
      break;
    case SCREEN_TSP_INTRO:
      draw_intro("TRAVELING SALESMAN PROBLEM",
                 "A salesman must visit all 6 cities and return home.\nWhat's "
                 "the SHORTEST tour that visits every city exactly once?",
                 "Compare: Brute Force (checks ALL tours) vs Branch & Bound "
                 "(prunes bad paths).\nSame answer, very different efficiency!",
                 C_ORANGE);
      break;
    case SCREEN_TSP:
      draw_tsp();
      break;
    case SCREEN_KNAPSACK_INTRO:
      draw_intro("0/1 KNAPSACK PROBLEM",
                 "Your truck can carry 50 units. You have cargo items with "
                 "different weights/values.\nWhat's the MOST valuable load you "
                 "can carry?",
                 "DP finds optimal integer solution. Greedy Fractional may get "
                 "higher value\nbut VIOLATES 0/1 rule (no splitting items)!",
                 C_GOLD);
      break;
    case SCREEN_KNAPSACK:
      draw_knapsack();
      break;
    }
    EndDrawing();
  }
  CloseWindow();
}
