#include "knapsack.h"
#include "benchmark.h"
#include <stdio.h>
#include <string.h>

/* -----------------------------------------
   SAMPLE CARGO -- called from main
----------------------------------------- */

void knapsack_load_sample(CargoItem items[], int *n) {
    *n = 8;
    /* name               weight  value(Rs thousands) */
    strcpy(items[0].name, "Engine Parts");    items[0].weight = 15; items[0].value = 30;
    strcpy(items[1].name, "Pharmaceuticals"); items[1].weight = 10; items[1].value = 40;
    strcpy(items[2].name, "Electronics");     items[2].weight = 20; items[2].value = 50;
    strcpy(items[3].name, "Textiles");        items[3].weight = 12; items[3].value = 20;
    strcpy(items[4].name, "Auto Components"); items[4].weight = 8;  items[4].value = 25;
    strcpy(items[5].name, "Chemicals");       items[5].weight = 18; items[5].value = 35;
    strcpy(items[6].name, "Food Grains");     items[6].weight = 25; items[6].value = 15;
    strcpy(items[7].name, "Medical Devices"); items[7].weight = 6;  items[7].value = 45;
}

/* -----------------------------------------
   ALGORITHM 1 -- 0/1 KNAPSACK  O(n*W)
   DP table approach -- OPTIMAL solution.
   Each item: either taken fully or not at all.
----------------------------------------- */

void knapsack_01(CargoItem items[], int n, int capacity) {
    /* dp[i][w] = max value using first i items, weight limit w */
    static int dp[MAX_ITEMS + 1][MAX_CAPACITY + 1];
    Timer t;

    timer_start(&t);

    for (int i = 0; i <= n; i++) {
        for (int w = 0; w <= capacity; w++) {
            t.operations++;
            if (i == 0 || w == 0) {
                dp[i][w] = 0;
            } else if (items[i - 1].weight <= w) {
                int take = items[i-1].value + dp[i-1][w - items[i-1].weight];
                int skip = dp[i-1][w];
                dp[i][w] = take > skip ? take : skip;
            } else {
                dp[i][w] = dp[i-1][w];
            }
        }
    }

    timer_stop(&t);

    /* backtrack to find which items were selected */
    int w = capacity;
    int selected[MAX_ITEMS];
    memset(selected, 0, sizeof(selected));

    for (int i = n; i > 0; i--) {
        if (dp[i][w] != dp[i-1][w]) {
            selected[i-1] = 1;
            w -= items[i-1].weight;
        }
    }

    /* print results */
    int total_w = 0, total_v = 0;
    printf("\n  +--- 0/1 KNAPSACK -- Dynamic Programming (OPTIMAL) ---+\n");
    printf("  |  Capacity   : %d units\n", capacity);
    printf("  |  Complexity : O(n*W)  |  n=%d items  W=%d\n", n, capacity);
    printf("  |  Each item  : Take it whole or leave it\n\n");
    printf("  %-20s  %8s  %10s\n", "Item", "Weight", "Value(Rsk)");
    printf("  %-20s  %8s  %10s\n", "----", "------", "---------");

    for (int i = 0; i < n; i++) {
        if (selected[i]) {
            printf("  %-20s  %8d  %10d  [LOADED]\n",
                   items[i].name, items[i].weight, items[i].value);
            total_w += items[i].weight;
            total_v += items[i].value;
        }
    }
    printf("  %-20s  %8d  %10d\n", "--- TOTAL ---", total_w, total_v);
    printf("  Max value (optimal): Rs%d thousand\n", dp[n][capacity]);
    timer_print(&t, "0/1 Knapsack DP");
}

/* -----------------------------------------
   ALGORITHM 2 -- FRACTIONAL KNAPSACK  O(n log n)
   Greedy -- sort by value/weight ratio,
   take fractions. OPTIMAL for fractional,
   but NOT for 0/1 -- we prove that below.
----------------------------------------- */

/* sort by value/weight ratio descending (insertion sort) */
static void sort_by_ratio(CargoItem sorted[], int n) {
    for (int i = 1; i < n; i++) {
        CargoItem key = sorted[i];
        double    kr  = (double)key.value / key.weight;
        int       j   = i - 1;
        while (j >= 0 &&
               (double)sorted[j].value / sorted[j].weight < kr) {
            sorted[j+1] = sorted[j];
            j--;
        }
        sorted[j+1] = key;
    }
}

void knapsack_fractional(CargoItem items[], int n, int capacity) {
    CargoItem sorted[MAX_ITEMS];
    memcpy(sorted, items, (size_t)n * sizeof(CargoItem));
    Timer t;

    timer_start(&t);
    sort_by_ratio(sorted, n);

    double total_value  = 0.0;
    int    total_weight = 0;

    /* store fractions for display */
    double fraction[MAX_ITEMS];
    memset(fraction, 0, sizeof(fraction));

    int rem = capacity;
    for (int i = 0; i < n && rem > 0; i++) {
        t.operations++;
        if (sorted[i].weight <= rem) {
            fraction[i]   = 1.0;
            rem          -= sorted[i].weight;
            total_weight += sorted[i].weight;
            total_value  += sorted[i].value;
        } else {
            fraction[i]   = (double)rem / sorted[i].weight;
            total_value  += fraction[i] * sorted[i].value;
            total_weight += rem;
            rem           = 0;
        }
    }

    timer_stop(&t);

    printf("\n  +--- FRACTIONAL KNAPSACK -- Greedy (OPTIMAL for fractions) ---+\n");
    printf("  |  Capacity   : %d units\n", capacity);
    printf("  |  Complexity : O(n log n)  -- dominated by sort\n");
    printf("  |  Each item  : Can be split into fractions\n\n");
    printf("  %-20s  %6s  %8s  %10s  %s\n",
           "Item", "W/unit", "V(Rsk)", "Ratio", "Taken");
    printf("  %-20s  %6s  %8s  %10s  %s\n",
           "----", "------", "------", "-----", "-----");

    for (int i = 0; i < n; i++) {
        if (fraction[i] > 0) {
            double ratio = (double)sorted[i].value / sorted[i].weight;
            if (fraction[i] >= 1.0)
                printf("  %-20s  %6d  %8d  %10.2f  100%%\n",
                       sorted[i].name, sorted[i].weight,
                       sorted[i].value, ratio);
            else
                printf("  %-20s  %6d  %8d  %10.2f  %.0f%% (split)\n",
                       sorted[i].name, sorted[i].weight,
                       sorted[i].value, ratio, fraction[i] * 100);
        }
    }
    printf("  %-20s  %6d  %10.2f\n", "--- TOTAL ---", total_weight, total_value);
    timer_print(&t, "Fractional Knapsack Greedy");
}

/* -----------------------------------------
   COMPARISON RUNNER -- called from main
   Runs both on same data, prints the gap
----------------------------------------- */

void knapsack_compare(int capacity) {
    CargoItem items[MAX_ITEMS];
    int n;
    knapsack_load_sample(items, &n);

    printf("\n  Cargo manifest (%d items, vehicle capacity: %d units):\n", n, capacity);
    printf("  %-20s  %8s  %10s  %8s\n",
           "Item", "Weight", "Value(Rsk)", "Ratio");
    printf("  %-20s  %8s  %10s  %8s\n",
           "----", "------", "---------", "-----");
    for (int i = 0; i < n; i++)
        printf("  %-20s  %8d  %10d  %8.2f\n",
               items[i].name, items[i].weight, items[i].value,
               (double)items[i].value / items[i].weight);

    knapsack_01(items, n, capacity);
    knapsack_fractional(items, n, capacity);
}
