#ifndef JACOBI_UTILS
#define JACOBI_UTILS

#include <cmath>
#include <iostream>
#include <vector>
#include <iomanip>

void print_usage(std::string name) {
	std::cout << "Usage: " << name
			  << " <n> <seed> <nw>\n"
				 "\tn\t dimension of the random matrix\n"
				 "\tseed\t seed of the random matrix (use 0 to use the timestamp)\n"
				 "\tnw\t number of workers"
			  << std::endl;
}

void print_matrix(std::vector<std::vector<double>> A, size_t n,
				  std::string name) {
	std::cout << "Matrix " << name << ":" << std::endl;
	for (size_t i = 0; i < n; i++) {
		for (size_t j = 0; j < n; j++) {
			std::cout << std::setw(4) << A[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

void print_vector(std::vector<double> a, std::string name) {
	std::cout << name << ": ";
	for (size_t i = 0; i < a.size(); i++) {
		std::cout << a[i] << " ";
	}
	std::cout << std::endl;
}

void create_matrix(std::vector<std::vector<double>> &A, size_t n) {
	for (size_t i = 0; i < (size_t)n; i++) {
		double sum = 0;
		for (size_t j = 0; j < (size_t)n; j++) {
			if (i != j) {
				A[i][j] = (rand() % 256) - 128;	 // To have numbers < 0
				sum += fabs(A[i][j]);
			}
		}
		// To make sure the matrix is diagonally dominant
		A[i][i] = (rand() % 128) + sum;
	}
}

#endif