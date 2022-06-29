#include <chrono>
#include <fstream>

#include "lib.cpp"

//#define DEBUG

#define INIT_TIME                                                   \
	auto start = std::chrono::high_resolution_clock::now();         \
	auto end   = std::chrono::high_resolution_clock::now() - start; \
	auto usec =                                                     \
		std::chrono::duration_cast<std::chrono::microseconds>(end).count();
#define BEGIN_TIME start = std::chrono::high_resolution_clock::now();
#define END_TIME                                              \
	end	 = std::chrono::high_resolution_clock::now() - start; \
	usec = std::chrono::duration_cast<std::chrono::microseconds>(end).count();

/******* SUPPORT FUNCTIONS *******/
bool is_number(char number[]) {
	for (size_t i = 0; number[i] != 0; i++) {
		if (!isdigit(number[i])) return false;
	}
	return true;
}

void print_usage(std::string name) {
	std::cout << "Usage: " << name
			  << " <matrix> <nw>"
				 "\n <matrix>\n    inputfile \tInput from "
				 "\"input.txt\"\n    <n> \t"
				 "Random matrix of dimension n, with tolerance 0 "
				 "and INT_MAX max_iterations"
				 "\n <nw> \tNumber of workers"
			  << std::endl;
}

/******* JACOBI FUNCTION *******/
void jacobi(std::vector<std::vector<double>> &A, std::vector<double> B,
			std::vector<double> &x, int max_iter, int nw) {
	size_t n = B.size();

	INIT_TIME;

	// SEQUENTIAL EXECUTION
	// Set x to 0
	std::fill(x.begin(), x.end(), 0);
	BEGIN_TIME;
	jacobi_seq(A, B, x, max_iter);
	END_TIME;
	auto seq_time = usec;
	std::cout << "Sequential: " << seq_time << " usecs" << std::endl;
#ifdef DEBUG
	std::cout << "Solution: " << std::endl;
	for (size_t i = 0; i < n; i++) {
		std::cout << "x" << i << " = " << x[i] << std::endl;
		if (i == n - 1) std::cout << std::endl;
	}
#endif

	// PARALLEL EXECUTION
	// Reset x
	std::fill(x.begin(), x.end(), 0);
	BEGIN_TIME;
	jacobi_par_threads(A, B, x, max_iter, nw);
	END_TIME;
	auto threads_time = usec;
	std::cout << "Threads:    " << threads_time << " usecs" << std::endl;
#ifdef DEBUG
	std::cout << "Solution: " << std::endl;
	for (size_t i = 0; i < n; i++) {
		std::cout << "x" << i << " = " << x[i] << std::endl;
		if (i == n - 1) std::cout << std::endl;
	}
#endif

	// FASTFLOW EXECUTION
	// Reset x
	std::fill(x.begin(), x.end(), 0);
	// 	BEGIN_TIME;
	//	jacobi_par_ff(A, B, x, max_iter, nw);
	// 	END_TIME;
	// 	auto ff_time = usec;
	// 	std::cout << "\nFastFlow:   " << ff_time << " usecs" << std::endl;
	// #ifdef DEBUG
	// 	std::cout << "Solution: " << std::endl;
	// 	for (size_t i = 0; i < n; i++) {
	// 		std::cout << "x" << i << " = " << x[i] << std::endl;
	// 		if (i == n-1) std::cout << std::endl;
	// 	}
	// #endif

	// Calculate speedups
	std::cout << "\nSPEEDUP:" << std::endl;
	std::cout << "Threads:  " << ((float)seq_time) / ((float)threads_time)
			  << std::endl;
	// 	std::cout << "FastFlow: " << ((float)seq_time) /
	// ((float)ff_time)
	// 			  << std::endl;
}

int main(int argc, char *argv[]) {
	// If no arguments are given, print usage
	if (argc < 3) {
		print_usage(argv[0]);
		return 0;
	}

	int nw = std::stoi(argv[2]);

	if (argv[1] == std::string("inputfile")) {
		// Read from input.txt
		std::ifstream file("input.txt");
		// Read n
		int n;
		file >> n;

		std::vector<std::vector<double>> A(n, std::vector<double>(n, 0));
		std::vector<double> B(n);
		std::vector<double> x(n);
		int max_iter;
		// Read A, B and max_iter
		for (size_t i = 0; i < n; i++) {
			for (size_t j = 0; j < n; j++) {
				file >> A[i][j];
			}
		}
		for (size_t i = 0; i < n; i++) {
			file >> B[i];
		}
		file >> max_iter;
#ifdef DEBUG
		print_matrix(A, n, "A");
		print_vector(B, "B");
		std::cout << "Max iterations: " << max_iter << std::endl << std::endl;
#endif

		// Check if matrix is diagonally dominant
		if (!check_diagonally_dominant(A, n)) {
			std::cerr
				<< "Jacobi cannot be executed on a non diagonally dominant "
				   "matrix, stopping..."
				<< std::endl;
			return (-1);
		}

		// Execute Jacobi
		jacobi(A, B, x, max_iter, nw);

	} else if (is_number(argv[1])) {
		int n = std::stoi(argv[1]);
		std::vector<std::vector<double>> A(n, std::vector<double>(n, 0));
		std::vector<double> B(n);
		std::vector<double> x(n);
		// double tol	 = 0;
		int max_iter = 1024;
		// Create matrix A and B
		srand(time(0));
		create_matrix(A, n);
		for (size_t i = 0; i < n; i++) {
			B[i] = (rand() % 256) - 128;
		}

#ifdef DEBUG
		print_matrix(A, n, "A");
		print_vector(B, "B");
		// std::cout << "Tolerance: " << tol << std::endl;
		std::cout << "Max iterations: " << max_iter << std::endl << std::endl;
#endif

		// Execute Jacobi
		jacobi(A, B, x, max_iter, nw);

	} else {
		std::cerr << "Unknown argument: " << argv[1] << std::endl;
		return 0;
	}

	return 0;
}