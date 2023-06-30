#include <iostream>
#include <random>
#include <cfloat>
#include <string.h>
using namespace std;
const inline size_t M = 10;
const inline size_t N = 2048;

void CopyMatrix(const float* from, float* to) {
	for (size_t i = 0; i < N; i++) {
		for (size_t j = 0; j < N; j++) {
			to[i * N + j] = from[i * N + j];
		}
	}

}

void SumMatrix(const float *M1, const float *M2, float *result) {
	for (size_t i = 0; i < N; i++) {
		for (size_t j = 0; j < N; j++) {
			result[i * N + j] = M1[i * N + j] + M2[i * N + j];
		}
	}
}

void showMatrix(float *matrix) {
	size_t i, j;
	for (i = 0; i < N; ++i) {
		for (j = 0; j < N; ++j) {
			cout << matrix[i * N + j] << " ";
		}
		cout << endl;
	}
	cout << "\n\n\n";
}

inline float generateRandomInRange(float a, float b) {
	random_device rd;
	uniform_real_distribution gen(a, b);
	return gen(rd);
}

float* GenerateMatrixI() {
	auto *I = new float[N * N];
	for (size_t i = 0; i < N; i++) {
		for (size_t j = 0; j < N; j++) {
			if (i == j) I[i * N + j] = 1.0;
			else I[i * N + j] = 0.0;
		}
	}
	return I;
}

float *GenerateMatrix() {
	auto *A = new float[N * N];
	for (size_t i = 0; i < N; i++) {
		for (size_t j = 0; j < N; j++) {
			A[i * N + j] = generateRandomInRange(1, 100000);
		}
	}

	return A;
}

void TransposeMatrix(float *B, const float *A) {
	for (size_t i = 0; i < N; i++)
		for (size_t j = 0; j < N; j++) {
			B[i * N + j] = A[j * N + i];
		}
}

float MaxLineCount(const float *matrix) {
	float maximum = FLT_MIN;
	for (size_t i = 0; i < N; i++) {
		float temp = 0;

		for (size_t j = 0; j < N; j++) {
			temp += matrix[i * N + j];
		}

		if (temp > maximum) {
			maximum = temp;
		}
	}
	return maximum;
}

float *GenerateMatrixB(float *A) {
	auto *B = new float[N * N];
	auto *transposedA = new float[N * N];
	TransposeMatrix(transposedA, A);
	float maxRowCount = MaxLineCount(A);
	float maxColumnCount = MaxLineCount(transposedA);
	float divider = 1 / (maxRowCount * maxColumnCount);
	for (size_t i = 0; i < N; i++) {
		for (size_t j = 0; j < N; j++) {
			B[i * N + j] = transposedA[i * N + j] * divider;
		}
	}
	delete[] transposedA;
	return B;
}

float *MultiplyMatrices(const float *M1, const float *M2) {
	auto* M3 = new float[N * N];
	memset(M3, 0, sizeof(float) * N * N);
	for (size_t i = 0; i < N; ++i) {
		float* c = M3 + i * N;
		for (size_t k = 0; k < N; ++k) {
			const float* b = M2 + k * N;
			float a = M1[i * N + k];
			for (size_t j = 0; j < N; ++j){
				c[j] += a * b[j];
			}
		}
	}
	return M3;
}

void SubtractMatrix(const float *I, const float *multed, float *R) {
	for (size_t i = 0; i < N; i++)
		for (size_t j = 0; j < N; j++)
			R[i * N + j] = I[i * N + j] - multed[i * N + j];
}

float *GenerateMatrixR(const float *A, float *I, const float *B) {
	auto *R = new float[N * N];
	auto *multed = MultiplyMatrices(B, A);
	// R - BA
	SubtractMatrix(I, multed, R);
	delete[] multed;
	return R;
}

float *GetInversedMatrix(float *A) {
	float *I = GenerateMatrixI();
	auto *copyI = new float[N*N];
	CopyMatrix(I, copyI);

	float *B = GenerateMatrixB(A);
//	std::cout << "B matrix" << std::endl;
//	showMatrix(B);

	float *R = GenerateMatrixR(A, copyI, B);
//	std::cout << "R matrix" << std::endl;
//	showMatrix(R);

	float *Rn = R;
	float *t;

	SumMatrix(I, Rn, I);
//	std::cout << "I matrix" << std::endl;
//	showMatrix(I);
	for (size_t i = 0; i < M; i++) {
		t = MultiplyMatrices(Rn, R);
//		std::cout << "T matrix " << i << " " << std::endl;
//		showMatrix(t);
		if (i != 0) delete[] Rn;
		Rn = t;
		SumMatrix(I, Rn, I);
//		std::cout << "I matrix" << std::endl;
//		showMatrix(I);
	}

	auto new_I = MultiplyMatrices(I, B);

	delete[] I;
	delete[] B;
	delete[] R;
	delete[] t;
	delete [] copyI;
	return new_I;
}

int main()
{
	float* A = GenerateMatrix();
	float* InversedA = GetInversedMatrix(A);
//	std::cout << "Matrix A" << std::endl;
//	showMatrix(A);

//	std::cout << "Inversed Matrix" << std::endl;
//	showMatrix(InversedA);

//	std::cout << "A*A^-1" << std::endl;
	showMatrix(MultiplyMatrices(A, InversedA));

	delete[] A;
	delete[] InversedA;
	return 0;
}