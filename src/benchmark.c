#include "benchmark.h"
#include <stdio.h>

void timer_start(Timer *t) {
    t->operations = 0;
    t->start = clock();
}

void timer_stop(Timer *t) {
    t->end = clock();
}

double timer_elapsed_ms(Timer *t) {
    return ((double)(t->end - t->start) / CLOCKS_PER_SEC) * 1000.0;
}

void timer_print(Timer *t, const char *label) {
    printf("\n  [BENCHMARK] %s\n", label);
    printf("  %-20s %.4f ms\n", "Time elapsed:", timer_elapsed_ms(t));
    printf("  %-20s %ld\n",     "Operations:  ", t->operations);
}
