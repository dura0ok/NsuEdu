#include <stdio.h>
#include "stdlib.h"

void swap(int *a, int *b) {
    int t;

    t = *a, *a = *b, *b = t;
}

void heap(int *a, int i, int n){
    int max = i;
    while(1){
        int child = 2 * i + 1;
        if(child < n && a[child] > a[max]) max = child;
        child++;
        if(child < n && a[child] > a[max]) max = child;
        if(max == i){
            break;
        }
        swap(&a[max], &a[i]);
        i = max;
    }
}

void heapsort(int *a, int n){
    for(int i = n / 2; i >= 0; i--)
        heap(a, i, n);
    for(int i = n-1; i >= 1; i--){
        swap(&a[0], &a[i]);
        heap(a, 0, i);
    }
}

int main() {
    int n;
    if(scanf("%d", &n) != 1){
        return 0;
    }
    int *arr = malloc(sizeof(int) * n);
    for (int i = 0; i < n; ++i) {
        int b;
        if(scanf("%d", &b) != 1){
            free(arr);
            return 0;
        }
        arr[i] = b;
    }
    heapsort(arr, n);
    for (int i = 0; i < n; ++i) {
        printf("%d ", arr[i]);
    }
    free(arr);
    return 0;
}
