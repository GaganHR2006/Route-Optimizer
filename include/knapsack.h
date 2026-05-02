#ifndef KNAPSACK_H
#define KNAPSACK_H

#define MAX_ITEMS 50

typedef struct {
    char name[50];
    int  weight;
    int  value;
} CargoItem;

void knapsack_01(CargoItem items[], int n, int capacity);
void knapsack_fractional(CargoItem items[], int n, int capacity);

#endif
