#include <barrier>
#include <cmath>
#include <iostream>
#include <thread>
#include <vector>

void print_matrix(std::vector<std::vector<double>> A, size_t n,
				  std::string name) {
	std::cout << "Matrix " << name << ":" << std::endl;
	for (size_t i = 0; i < n; i++) {
		for (size_t j = 0; j < n; j++) {
			std::cout << A[i][j] << " ";
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

bool check_diagonally_dominant(std::vector<std::vector<double>> A, size_t n) {
	for (size_t i = 0; i < n; i++) {
		double row_sum = 0;
		for (size_t j = 0; j < n; j++) {
			if (i != j) {
				row_sum += abs(A[i][j]);
			}
		}
		if (row_sum > abs(A[i][i])) {
			return false;
		}
	}
	return true;
}

void create_matrix(std::vector<std::vector<double>> &A, size_t n) {
	for (size_t i = 0; i < n; i++) {
		double sum = 0;
		for (size_t j = 0; j < n; j++) {
			if (i != j) {
				A[i][j] = (rand() % 256) - 128;	 // To have numbers < 0
				sum += fabs(A[i][j]);
			}
		}
		// To make sure the matrix is diagonally dominant
		A[i][i] = (rand() % 128) + sum;
	}
}

void jacobi_seq(std::vector<std::vector<double>> &A, std::vector<double> B,
				std::vector<double> &x_old, int max_iter) {
	size_t n = B.size();
	std::vector<double> x_new(n);
	for (size_t k = 0; k < max_iter; k++) {
		for (size_t i = 0; i < n; i++) {
			// Find the sum of all values except the one on the diagonal
			double sum_j = 0;
			for (size_t j = 0; j < n; j++) {
				if (i != j) {
					sum_j += A[i][j] * x_old[j];
				}
			}
			// Update the value of x
			x_new[i] = (1 / A[i][i]) * (B[i] - sum_j);
		}

		// Update x_old
		x_old = x_new;
	}
}

void jacobi_par_threads(std::vector<std::vector<double>> &A,
						std::vector<double> B, std::vector<double> &x_old,
						int max_iter, int nw) {
	size_t n = B.size();
	std::vector<double> x_new(n);
	std::vector<std::thread> threads(nw);

	// Create the barrier
	std::barrier sync_point(nw, [&]() {
		max_iter--;
		// Update x_old
		x_old = x_new;
	});

	int chunk_size = int(n / nw);
	int start	   = 0;
	int end		   = 0;

	// Create the pair of indices for the ranges of the threads
	std::vector<std::pair<size_t, size_t>> ranges(nw);
	for (size_t i = 0; i < nw; i++) {
		start = i == 0 ? 0 : end + 1;
		end	  = start + chunk_size;
		if (end >= n) end = n - 1;
		ranges[i] = std::make_pair(start, end);
	}

	// Launch the threads
	for (size_t i = 0; i < nw; i++) {
		threads[i] = std::thread(
			[&](std::pair<size_t, size_t> range) {
				while (max_iter > 0) {
					// Iterate on the rows of the selected chunk
					for (size_t j = range.first; j <= range.second; j++) {
						// Find the sum of all values except the one on the
						// diagonal
						double sum_j = 0;
						// row=j, column=l
						for (size_t l = 0; l < n; l++) {
							if (l != j) {
								sum_j += A[j][l] * x_old[l];
							}
						}
						// Update the value of x_new
						x_new[j] = (1 / A[j][j]) * (B[j] - sum_j);
					}
					sync_point.arrive_and_wait();
				}
			},
			ranges[i]);
	}

	// Join the threads
	for (auto &t : threads) {
		t.join();
	}
}
