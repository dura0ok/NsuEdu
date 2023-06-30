#include <iostream>
#include <random>
#include <cfloat>
#include <iomanip>
#include "cblas.h"
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


void showMatrix(const float *matrix) {
	size_t i, j;
	for (i = 0; i < N; ++i) {
		for (j = 0; j < N; ++j) {
			const auto* x = &matrix[i * N + j];

			std::cout.setf(std::ios::fixed);
			std::cout << std::setprecision(3);
			std::cout << std::setw(6);
//			if (*x < 0.0001){
//				std::cout << 0;
//			}
//			else{
//				cout << *x;
//			}
			std::cout << *x;
			std::cout << ' ';
		}
		cout << endl;
	}
	cout << "\n\n\n";
}

void SumMatrix(const float *M1, float *M2) {
	cblas_saxpy(N*N, 1, M1, 1, M2, 1);
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
	/*
	 * A - матрица M*K
	   B - матрица K*N
	   M, N, K, alpha, A, lda=K, ldb, ldc=N число элементов в ведущей размерности матрицы (строке или столбце)
	   B, ldb, beta, C, ldc
	   С = αAB + βC

	 */
	cblas_sgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans, N,N,N,1.0,M1,N,M2,N,0.0,M3,N);
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
	float *R = GenerateMatrixR(A, copyI, B);


	float *Rn = R;
	float *t;


	SumMatrix(Rn, I);

	for (size_t i = 0; i < M; i++) {
		t = MultiplyMatrices(Rn, R);
		if (i != 0) delete[] Rn;
		Rn = t;
		SumMatrix(Rn, I);
	}

	auto new_I = MultiplyMatrices(I, B);

	delete[] I;
	delete[] B;
	delete[] R;
	delete[] t;
	delete [] copyI;
	return new_I;
}

int main() {
	float *A = GenerateMatrix();
	float *InversedA = GetInversedMatrix(A);
	showMatrix(MultiplyMatrices(A,InversedA));

	delete[] A;
	delete[] InversedA;
	return 0;
}