#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <random>
using namespace std;

constexpr inline int MIN_N = 1024 / sizeof(int);
constexpr inline int MAX_N = 33554432*2 / sizeof(int); // 64 MB
constexpr inline int MATRIX_N = 1024;


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


void gen_forward(int* data, const int iterations_count) {
  for (int i = 0; i < iterations_count - 1; i++) {
	data[i] = i + 1;
  }
  data[iterations_count - 1] = 0;
}

void gen_backward(int* data, const int N) {
  data[0] = N - 1;
  for (int i = 1; i < N; i++) {
	data[i] = i - 1;
  }
}

void gen_random(int* data, const int N) {
  int* perm = (int*)malloc(N * sizeof(int));
  for(int i = 0; i < N; i++){
	perm[i] = i;
  }
  for (int i = N - 1; i >= 0; i--){
	int j = rand() % (i + 1);
	std::swap(perm[i], perm[j]);
  }
  for (int i = 0; i < N; i++) {
	data[perm[i]] = perm[(i + 1) % N];
  }
  free(perm);
}


int test(int* data, const int N, const size_t iterations_count, void (*gen)(int* data, const int N)) {
  gen(data, N);
  int k;
  long long i;

  for (k = 0, i = 0; i < N; i++) {
	k = data[k];
  }

  if (k==123456789) printf("Wow!");

  for (k = 0, i = 0; i < N; i++) {
	k = data[k];
  }

  if (k==123456789) printf("Wow!");

  unsigned long long start = __builtin_ia32_rdtsc();

  for (k = 0, i = 0; i < N*iterations_count; i++) {
	k = data[k];
  }

  unsigned long long end = __builtin_ia32_rdtsc();
  if (k==123456789) printf("Wow!");
  printf(" %d, %zu, %0.2lf\n", N*4, iterations_count, (((end-start))*1.0 / N*1.0) / iterations_count);
  return k;
}

int main(int argc, char *argv[]){

  auto *matrix_1 = GenerateMatrix();
  auto *matrix_2 = GenerateMatrix();
  auto *matrix_3 = MultiplyMatrices(matrix_1, matrix_2);
  if(matrix_3[0] == 12345689){
	printf("wow");
  }
  if (argc < 2) return 1;
  auto K = (size_t) (atoi(argv[1]));

  int* data = (int*)malloc(MAX_N * sizeof(int));
  for (int N = MIN_N; N <= MAX_N; N *= 1.2) {
	printf("forward,");
	test(data, N, K, gen_forward);
	printf("backward,");
	test(data, N, K, gen_backward);
	printf("random,");
	test(data, N, K, gen_random);
  }

  free(data);
  return 0;
}