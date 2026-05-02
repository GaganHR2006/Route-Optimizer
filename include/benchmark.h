#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <time.h>

typedef struct {
    clock_t start;
    clock_t end;
    long    operations; // manual op counter
} Timer;

void   timer_start(Timer *t);
void   timer_stop(Timer *t);
double timer_elapsed_ms(Timer *t);
void   timer_print(Timer *t, const char *label);

#endif
