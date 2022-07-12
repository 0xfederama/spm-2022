#include <barrier>
#include <ff/ff.hpp>
#include <ff/parallel_for.hpp>
#include <thread>

#include "utils.cpp"

const int max_iter = 128;

void jacobi_seq(std::vector<std::vector<double>> &A, std::vector<double> B,
				std::vector<double> &x_old) {
	size_t n = B.size();
	std::vector<double> x_new(n);
	for (size_t k = 0; k < (size_t)max_iter; k++) {
		for (size_t i = 0; i < (size_t)n; i++) {
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

void jacobi_par_std(std::vector<std::vector<double>> &A,
						std::vector<double> B, std::vector<double> &x_old,
						int nw) {
	size_t n = B.size();
	std::vector<double> x_new(n);
	std::vector<std::thread> threads(nw);

	// Create the barrier
	int iter = max_iter;
	std::barrier sync_point(nw, [&]() {
		iter--;
		// Update x_old
		x_old = x_new;
	});

	int chunk_size = int(n / nw);
	int start	   = 0;
	int end		   = 0;

	// Create the pair of indices for the ranges of the threads
	std::vector<std::pair<size_t, size_t>> ranges(nw);
	for (size_t i = 0; i < (size_t)nw; i++) {
		start = i == 0 ? 0 : end + 1;
		end	  = start + chunk_size;
		if ((size_t)end >= n) end = n - 1;
		ranges[i] = std::make_pair(start, end);
	}

	// Launch the threads
	for (size_t i = 0; i < (size_t)nw; i++) {
		threads[i] = std::thread(
			[&](std::pair<size_t, size_t> range) {
				while (iter > 0) {
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

void jacobi_par_ff(std::vector<std::vector<double>> &A, std::vector<double> B,
				   std::vector<double> &x_old, int nw) {
	size_t n = B.size();
	std::vector<double> x_new(n);
	std::vector<std::thread> threads(nw);

	// Create ParallelFor instance
	ff::ParallelFor par_for(nw);

	for (size_t k = 0; k < (size_t)max_iter; k++) {
		// Parallel for
		par_for.parallel_for(
			0, n, 1, 0,
			[&](size_t i) {
				// Jacobi pass
				double sum_j = 0;
				for (size_t j = 0; j < (size_t)n; j++) {
					if (i != j) {
						sum_j += A[i][j] * x_old[j];
					}
				}
				x_new[i] = (1 / A[i][i]) * (B[i] - sum_j);
			},
			nw);

		// Update x_old
		x_old = x_new;
	}
}