#include <stdio.h>
#include "stdlib.h"

void printArray(int *array, int n)
{
    for (int i = 0; i < n; ++i)
        printf("%d ", array[i]);
}

void quickSort(int *array, int low, int high)
{
    int i = low;
    int j = high;
    int pivot = array[(i + j) / 2];
    int temp;

    while (i <= j)
    {
        while (array[i] < pivot)
            i++;
        while (array[j] > pivot)
            j--;
        if (i <= j)
        {
            temp = array[i];
            array[i] = array[j];
            array[j] = temp;
            i++;
            j--;
        }
    }
    if (j > low)
        quickSort(array, low, j);
    if (i < high)
        quickSort(array, i, high);
}

int main() {
    int n;
    if(scanf("%d", &n) != 1){
        return 0;
    }
    if(n == 0){
        return 0;
    }
    int *array = malloc(sizeof(int) * n);
    if(array == NULL) return 0;
    for (int i = 0; i < n; ++i) {
        if(scanf("%d", &array[i]) != 1){
            free(array);
            return 0;
        }
    }


    quickSort(array, 0, n-1);

    printArray(array, n);

    free(array);
    return (0);
}
