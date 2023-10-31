#include <iostream>
#include <chrono>
#include <random>
#include <thread>

#define MATRIX_SIZE 1152
#define THREADS_NUMBER 32
#define SIZE_STEP 64

//====DEBUG=======
void ini1234(double** M, const int n, bool zero = false)
{
	for (int i = 0; i < n; ++i)
	{
		M[i] = new double[n];
	}
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			M[i][j] = zero ? 0. : (i + 1) * (j + 2);
		}
	}
}
void ini4321(double** M, const int n)
{
	for (int i = 0; i < n; ++i)
	{
		M[i] = new double[n];
	}
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			M[i][j] = (i + 3) * (-j - 6);
		}
	}
}
void printM(double** M, const int n)
{
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			std::cout << M[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "\n";
}
//================

void init(double** M, const int n, bool zero = false)
{
	std::random_device rd;
	std::mt19937 generator(rd());

	double min = -50.;
	double max = 50.;

	std::uniform_real_distribution<double> distribution(min, max);

	for (int i = 0; i < n; ++i)
	{
		M[i] = new double[n];
	}
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			M[i][j] = zero ? 0.: distribution(generator);
		}
	}
}
std::chrono::duration<double> multiply(const int n)
{
	double** A = new double* [n];
	double** B = new double* [n];
	double** C = new double* [n];

	init(A, n);
	init(B, n);
	/*ini1234(A, n);
	ini4321(B, n);*/
	init(C, n, true);

	auto start_time = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < n; ++i) {
		for (int k = 0; k < n; ++k) {
			for (int j = 0; j < n; ++j) {
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}
	auto end_time = std::chrono::high_resolution_clock::now();
	//std::cout << "C mono" << "\n";
	//printM(C, n);
	delete[]A;
	delete[]B;
	delete[]C;

	return end_time - start_time;
}
void multiply_threading(int start, int end, double **A, double** B, double** C, const int n)
{
	for (int i = start; i < end; ++i) {
		for (int k = 0; k < n; ++k) {
			for (int j = 0; j < n; ++j) {
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}
}
std::chrono::duration<double>multithreading_execution(const int n, const int threads_number)
{
	double** A = new double* [n];
	double** B = new double* [n];
	double** C = new double* [n];

	init(A, n);
	init(B, n);
	init(C, n, true);
	//ini1234(A, n);
	//ini4321(B, n);
	init(C, n, true);

	std::thread* threads = new std::thread[threads_number];

	const int iter_per_thread = n / threads_number;
	const int iter_per_thread_rest = n % threads_number;
	int start_op = 0;
	int end_op;
	for (int i = 0; i < threads_number; ++i) {
		end_op = start_op + iter_per_thread;
		if (i == threads_number - 1) end_op += iter_per_thread_rest;
		threads[i] = std::thread(multiply_threading, start_op, end_op, A, B, C, n);
		start_op = end_op;
	}

	auto start_time = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < threads_number; ++i) {
		threads[i].join();
	}
	auto end_time = std::chrono::high_resolution_clock::now();
	/*std::cout << "C multi" << "\n";
	printM(C, n);*/
	delete[]A;
	delete[]B;
	delete[]C;

	return end_time - start_time;
}
int main()
{
	std::cout << "\nMONOthread from matrix size\n";

	for (int i = SIZE_STEP; i <= MATRIX_SIZE; i+=SIZE_STEP)
		std::cout << multiply(i).count() << ", ";

	std::cout << "\n\nMULTIthread from matrix size\n";
	for (int i = SIZE_STEP; i <= MATRIX_SIZE; i += SIZE_STEP)
		std::cout << multithreading_execution(i, 16).count() << ", ";

	std::cout << "\n\nMULTIthread from number of threads\n";
	for (int i = 1; i <= THREADS_NUMBER; ++i)
		std::cout << multithreading_execution(MATRIX_SIZE, i).count() << ", ";

	return 0;
}