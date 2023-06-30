#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int generate_random_in_range(int lower, int upper) {
  return (rand() % (upper - lower + 1)) + lower;
}

void fill_array_random_numbers(size_t n, int *arr) {
  for (size_t i = 0; i < n; ++i) {
    arr[i] = generate_random_in_range(1, 10000);
  }
}

void bubble_sort(int *arr, int size) {
  for (int i = 0; i < size - 1; i++) {
    for (int j = (size - 1); j > i; j--) // for all elements after i
    {
      if (arr[j - 1] > arr[j]) // if this greater previous
      {
        int temp = arr[j - 1]; // swap it
        arr[j - 1] = arr[j];
        arr[j] = temp;
      }
    }
  }
}

void print_array(int *arr, int size) {
  for (size_t i = 0; i < size; i++) {
    printf("%d ", arr[i]);
    printf("\n");
  }
}

int main(int argc, char *argv[]) {
  struct timespec start, end;
  srand(time(0));
  if (argc < 2) return 1;
  size_t n = (size_t) (atoi(argv[1]));

  int *arr = malloc(sizeof(int) * n);
  fill_array_random_numbers(n, arr);
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);
  bubble_sort(arr, n);
  clock_gettime(CLOCK_MONOTONIC_RAW, &end);
  printf("%f",
            end.tv_sec-start.tv_sec
  + 0.000000001*(end.tv_nsec-start.tv_nsec));


  free(arr);
  return 0;
}

