#include <iostream>
#include <climits>
#include <random>
#include <cstring>
using namespace std;

const int RUN_TIMES = 50;
const inline int OFFSET = 4*1024 + 8;
const inline int FRAGMENTS_NUM = 32;
const inline int N = OFFSET*FRAGMENTS_NUM;

constexpr inline int MATRIX_N = 1024;

inline float generateRandomInRange(float a, float b) {
  random_device rd;
  uniform_real_distribution gen(a, b);
  return gen(rd);
}

float *GenerateMatrix() {
  auto *A = new float[MATRIX_N * MATRIX_N];
  for (size_t i = 0; i < MATRIX_N; i++) {
	for (size_t j = 0; j < MATRIX_N; j++) {
	  A[i * MATRIX_N + j] = generateRandomInRange(1, 100000);
	}
  }

  return A;
}

float *MultiplyMatrices(const float *M1, const float *M2) {
  auto* M3 = new float[MATRIX_N * MATRIX_N];
  memset(M3, 0, sizeof(float) * MATRIX_N * MATRIX_N);
  for (size_t i = 0; i < MATRIX_N; ++i) {
	float* c = M3 + i * MATRIX_N;
	for (size_t k = 0; k < MATRIX_N; ++k) {
	  const float* b = M2 + k * MATRIX_N;
	  float a = M1[i * MATRIX_N + k];
	  for (size_t j = 0; j < MATRIX_N; ++j){
		c[j] += a * b[j];
	  }
	}
  }
  return M3;
}

void init_array(int *arr, const size_t fragments) {
  for (size_t i = 0; i < N - OFFSET * (fragments - 1) ; i++) {
	for (size_t j = 0; j < fragments - 1; j++) {
	  arr[i + j*OFFSET] =  static_cast<int>(i + (j + 1) * OFFSET);
	}
	arr[i + OFFSET * (fragments - 1)] = i + 1;
  }
}

unsigned long long runLoop(int *arr, size_t fragments) {
  init_array(arr, fragments);
  auto minTime = ULLONG_MAX;
  for (size_t iterations_count = 0; iterations_count < RUN_TIMES; iterations_count++) {
	auto start = __builtin_ia32_rdtsc();
	for (volatile size_t k = 0, i = 0; i < N; i++) {
	  k = arr[k];
	}
	auto end = __builtin_ia32_rdtsc();
	if (minTime > end - start) {
	  minTime = end - start;
	}
  }

  return minTime;
}

void countTime(int *arr, int fragments){
  init_array(arr, fragments);
  std::cout << fragments << " " << runLoop(arr, fragments) / N << std::endl;
}

int main() {
	auto *matrix_1 = GenerateMatrix();
  auto *matrix_2 = GenerateMatrix();
  auto *matrix_3 = MultiplyMatrices(matrix_1, matrix_2);
  if(matrix_3[0] == 12345689){
	printf("wow");
  }
  auto *array = new int[N];

  for (size_t fragments = 1; fragments <= N/OFFSET; fragments++) {
	countTime(array, static_cast<int>(fragments));
  }

  delete[] array;
  return 0;
}
