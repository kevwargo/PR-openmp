

// Celem tego programu jest prezentacja pomiaru i analizy 
//efektywnosci programu za pomoc¹  CodeAnalyst(tm).
// Implementacja mno¿enia macierzy jest realizowana za pomoca typowego 
// algorytmu podrêcznikowego. 
#include <cstdio>
#include <ctime>
#include "omp.h"
#include <string>
#include <iostream>
#include <Windows.h>
using namespace std;

double start;

static const int n = 3000;
static const int ROWS =n;     // liczba wierszy macierzy
static const int COLUMNS = n;  // lizba kolumn macierzy

float matrix_a[ROWS][COLUMNS];    // lewy operand 
float matrix_b[ROWS][COLUMNS];    // prawy operand
float matrix_r[ROWS][COLUMNS];    // wynik

FILE *result_file;
void initialize_matrices();
void initialize_matricesZ();
void print_result();
void print_elapsed_time();
void multiply_matrices_IJK_serial()
{
	
		for (int i = 0; i < ROWS; i++) {
			for (int j = 0; j < COLUMNS; j++) {
				float sum = 0.0;
				for (int k = 0; k < COLUMNS; k++) {
					sum = sum + matrix_a[i][k] * matrix_b[k][j];
				}
				matrix_r[i][j] = sum;
			}
		}
	
}
void multiply_matrices_IJK_parallel()
{
	#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < ROWS; i++) {
			for (int j = 0; j < COLUMNS; j++) {
				float sum = 0.0;
				for (int k = 0; k < COLUMNS; k++) {
					sum = sum + matrix_a[i][k] * matrix_b[k][j];
				}
				matrix_r[i][j] = sum;
			}
		}
}
void multiply_matrices_IKJ_affinity()
{
#pragma omp parallel 
	{
		SetThreadAffinityMask(GetCurrentThread(), 1 << omp_get_thread_num());
	#pragma omp for schedule(dynamic)
		for (int i = 0; i < ROWS; i++)
			for (int k = 0; k < COLUMNS; k++)
				for (int j = 0; j < COLUMNS; j++)
					matrix_r[i][j] += matrix_a[i][k] * matrix_b[k][j];
		
	}
}
void multiply_matrices_IJK_affinity()
{
#pragma omp parallel 
	{
		SetThreadAffinityMask(GetCurrentThread(), 1 << omp_get_thread_num());
#pragma omp for schedule(dynamic)
		for (int i = 0; i < ROWS; i++) {
			for (int j = 0; j < COLUMNS; j++) {
				float sum = 0.0;
				for (int k = 0; k < COLUMNS; k++) {
					sum = sum + matrix_a[i][k] * matrix_b[k][j];
				}
				matrix_r[i][j] = sum;
			}
		}

	}
}
	

void multiply_matrices_IJK_prefetch( )
{
	
#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < ROWS; i++) {
			for (int j = 0; j < COLUMNS; j++) {
				float sum = 0.0;
				for (int k = 0; k < COLUMNS; k++) {
					_mm_prefetch((char*)(&matrix_b[k+5][j]), _MM_HINT_NTA);
					sum = sum + matrix_a[i][k] * matrix_b[k][j];
				}
				matrix_r[i][j] = sum;
			}
		}
	

	
}
void multiply_matrices_IKJ_serial( )
{
	for (int i = 0; i < ROWS; i++)
		for (int k = 0; k < COLUMNS; k++)
			for (int j = 0; j < COLUMNS; j++)
				matrix_r[i][j] += matrix_a[i][k] * matrix_b[k][j];
	
}

void multiply_matrices_IKJ_parallel()
{
	#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < ROWS; i++)
			for (int k = 0; k < COLUMNS; k++)
				for (int j = 0; j < COLUMNS; j++)
					matrix_r[i][j] += matrix_a[i][k] * matrix_b[k][j];

}


void test(void fun(),string name)
{
	initialize_matrices();
	start = (double)clock() / CLK_TCK;
	fun();
	cout << name <<endl;
	print_elapsed_time();
}
int main(int argc, char* argv[])
{
	
	omp_set_num_threads(4);
	///test(multiply_matrices_IJK_serial, "IJK_serial");
	test(multiply_matrices_IJK_parallel, "IJK_parallel");
	test(multiply_matrices_IJK_prefetch, "IJKPrefetch");
	//test(multiply_matrices_IJK_affinity, "IJK_affinity");
	//test(multiply_matrices_IKJ_serial, "IKJ_serial");
	//test(multiply_matrices_IKJ_parallel, "IKJ_paralel");
	//test(multiply_matrices_IKJ_affinity, "IKJ_affinity");
	

	return 0;
}



void initialize_matrices()
{
	// zdefiniowanie zawarosci poczatkowej macierzy
	//#pragma omp parallel for 
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLUMNS; j++) {
			matrix_a[i][j] = (float)rand() / RAND_MAX;
			matrix_b[i][j] = (float)rand() / RAND_MAX;
			matrix_r[i][j] = 0.0;
		}
	}

}
void initialize_matricesZ()
{
	// zdefiniowanie zawarosci poczatkowej macierzy
#pragma omp parallel for 
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLUMNS; j++) {
			matrix_r[i][j] = 0.0;
		}
	}
}


void print_elapsed_time()
{
	double elapsed;
	double resolution;

	// wyznaczenie i zapisanie czasu przetwarzania
	elapsed = (double)clock() / CLK_TCK;
	resolution = 1.0 / CLK_TCK;
	printf("Czas: %8.4f sec \n",
		elapsed - start);


}
