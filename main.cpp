#include <chrono>

#include "jacobi.cpp"
#include "utils.cpp"

#define INIT_TIME                                                   \
	auto start = std::chrono::high_resolution_clock::now();         \
	auto end   = std::chrono::high_resolution_clock::now() - start; \
	auto usec =                                                     \
		std::chrono::duration_cast<std::chrono::microseconds>(end).count();
#define BEGIN_TIME start = std::chrono::high_resolution_clock::now();
#define END_TIME                                              \
	end	 = std::chrono::high_resolution_clock::now() - start; \
	usec = std::chrono::duration_cast<std::chrono::microseconds>(end).count();

int main(int argc, char *argv[]) {
	// If no arguments are given, print usage
	if (argc < 4) {
		print_usage(argv[0]);
		return 0;
	}

	// Parse arguments
	int n	 = std::stoi(argv[1]);
	int seed = std::stoi(argv[2]);
	int nw	 = std::stoi(argv[3]);
	if (nw > n) {
		std::cerr << "Number of workers must be smaller than the size of "
					 "the matrix."
				  << std::endl;
		return (-1);
	}

	std::vector<std::vector<double>> A(n, std::vector<double>(n, 0));
	std::vector<double> B(n);
	std::vector<double> x(n);
	// Create matrix A and B
	if (seed != 0) srand(seed);
	else srand(time(0));
	create_matrix(A, n);
	for (size_t i = 0; i < (size_t)n; i++) {
		B[i] = (rand() % 256) - 128;
	}

#ifdef DEBUG
	print_matrix(A, n, "A");
	print_vector(B, "B");
	std::cout << std::endl;
#endif

	// Execute Jacobi
	INIT_TIME;

	// SEQUENTIAL EXECUTION
#ifdef JACOBI_SEQ
	// Set x to 0
	std::fill(x.begin(), x.end(), 0);
	BEGIN_TIME;
	jacobi_seq(A, B, x);
	END_TIME;
	auto seq_time = usec;
	std::cout << "Sequential: " << seq_time << " usecs" << std::endl;
#ifdef DEBUG
	std::cout << "Solution: " << std::endl;
	for (size_t i = 0; i < (size_t)n; i++) {
		std::cout << "x" << i << " = " << x[i] << std::endl;
	}
	std::cout << std::endl;
#endif
#endif

	// PARALLEL EXECUTION
#ifdef JACOBI_STD
	// Reset x
	std::fill(x.begin(), x.end(), 0);
	BEGIN_TIME;
	jacobi_par_std(A, B, x, nw);
	END_TIME;
	auto threads_time = usec;
	std::cout << "Threads:    " << threads_time << " usecs" << std::endl;
#ifdef DEBUG
	std::cout << "Solution: " << std::endl;
	for (size_t i = 0; i < (size_t)n; i++) {
		std::cout << "x" << i << " = " << x[i] << std::endl;
	}
	std::cout << std::endl;
#endif
#endif

	// FASTFLOW EXECUTION
#ifdef JACOBI_FF
	// Reset x
	std::fill(x.begin(), x.end(), 0);
	BEGIN_TIME;
	jacobi_par_ff(A, B, x, nw);
	END_TIME;
	auto ff_time = usec;
	std::cout << "FastFlow:   " << ff_time << " usecs" << std::endl;
#ifdef DEBUG
	std::cout << "Solution: " << std::endl;
	for (size_t i = 0; i < (size_t)n; i++) {
		std::cout << "x" << i << " = " << x[i] << std::endl;
	}
	std::cout << std::endl;
#endif
#endif

	return 0;
}