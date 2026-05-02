#ifndef KNAPSACK_H
#define KNAPSACK_H

#define MAX_ITEMS    50
#define MAX_CAPACITY 100

typedef struct {
    char name[50];
    int  weight;
    int  value;
} CargoItem;

void knapsack_load_sample(CargoItem items[], int *n);
void knapsack_01(CargoItem items[], int n, int capacity);
void knapsack_fractional(CargoItem items[], int n, int capacity);
void knapsack_compare(int capacity);

#endif
