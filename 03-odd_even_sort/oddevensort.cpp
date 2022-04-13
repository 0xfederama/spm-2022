#include <atomic>
#include <barrier>
#include <chrono>
#include <iostream>
#include <syncstream>
#include <thread>
#include <vector>

using namespace std;

#define INIT_TIME                                                              \
	auto start = std::chrono::high_resolution_clock::now();                    \
	auto elapsed = std::chrono::high_resolution_clock::now() - start;          \
	auto usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed) \
					.count();
#define BEGIN_TIME start = std::chrono::high_resolution_clock::now();
#define END_TIME                                                          \
	elapsed = std::chrono::high_resolution_clock::now() - start;          \
	usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed) \
			   .count();

// Sequential algorithm for odd even sort
void seq_oesort(vector<int>& v) {
	int n = v.size();
	bool sorted = false;
	while (!sorted) {
		sorted = true;
		for (int i = 1; i < n - 1; i += 2)	// Odd elements
			if (v[i] > v[i + 1]) {
				auto tmp = v[i];
				v[i] = v[i + 1];
				v[i + 1] = tmp;
				sorted = false;
			}
		for (int i = 0; i < n - 1; i += 2)	// Even elements
			if (v[i] > v[i + 1]) {
				auto tmp = v[i];
				v[i] = v[i + 1];
				v[i + 1] = tmp;
				sorted = false;
			}
	}
}

// OpenMP algorithm for odd even sort
void openmp_oesort(vector<int>& v, int nw) {
	int n = v.size();
	bool sorted = false;
	while (!sorted) {
		sorted = true;
#pragma omp parallel for num_threads(nw)
		for (int i = 1; i < n - 1; i += 2)	// Odd elements
			if (v[i] > v[i + 1]) {
				auto tmp = v[i];
				v[i] = v[i + 1];
				v[i + 1] = tmp;
				sorted = false;
			}
#pragma omp parallel for num_threads(nw)
		for (int i = 0; i < n - 1; i += 2)	// Even elements
			if (v[i] > v[i + 1]) {
				auto tmp = v[i];
				v[i] = v[i + 1];
				v[i + 1] = tmp;
				sorted = false;
			}
	}
}

// Parallel algorithm for odd even sort
void par_oesort(vector<int>& v, int nw) {
	atomic<bool> sorted = false;
	barrier ba(nw, [&]() {
		sorted = true;
		return;
	});
	barrier bb(nw, [&]() { return; });

	vector<pair<int, int>> chunks(nw);
	auto n = v.size();
	auto d = n / nw;  // assume is multiple (e.g. n = 2^k)
	for (int i = 0; i < nw; i++) {
		auto start = i * d;
		auto stop = (i == (nw - 1) ? n : d * (i + 1));
		chunks[i] = make_pair(start, stop);
	}
	atomic<int> globaliters;
	globaliters = 0;

	auto body = [&](int ti) {
		auto start = chunks[ti].first;
		auto stop = chunks[ti].second;
		auto last = (ti == (nw - 1));
		if (last) stop = chunks[ti].second - 1;
		auto iters = 0;

		while (!sorted) {
			iters++;
			// odd step
			auto localsorted = true;
			for (int i = start + 1; i < stop; i += 2)
				if (v[i] > v[i + 1]) {
					auto temp = v[i];
					v[i] = v[i + 1];
					v[i + 1] = temp;
					localsorted = false;
				}
			// cout << iters << endl;
			// wait all
			ba.arrive_and_wait();
			// even step, starts with global sorted true
			for (int i = start; i < stop; i += 2)
				if (v[i] > v[i + 1]) {
					auto temp = v[i];
					v[i] = v[i + 1];
					v[i + 1] = temp;
					localsorted = false;
				}
			// wait all
			if (localsorted == false)  // first update global sorted
				if (sorted) {
					sorted = false;
				}
			// cout << iters << "HERE" << endl;
			bb.arrive_and_wait();  // then barrier
								   // go to next iteration
		}
		globaliters = iters;
		// cout << iters << endl;
		return;
	};

	// now create threads
	vector<thread*> tids(nw);
	for (int i = 0; i < nw; i++) {
		tids[i] = new thread(body, i);
	}
	// and await their termination
	for (int i = 0; i < nw; i++) {
		tids[i]->join();
	}
}

int main(int argc, char* argv[]) {
	if (argc <= 1) {
		cout << "Usage: " << argv[0] << " seed n nw" << endl;
		return 0;
	}
	int seed = atoi(argv[1]);
	int n = atoi(argv[2]);
	int nw = atoi(argv[3]);

	const int max = 1024;
	if (seed != 0) srand(seed);
	vector<int> v(n), r1(n), r2(n), r3(n);
	// Create a vector of random numbers
	for (int i = 0; i < n; i++) {
		v[i] = rand() % max;
	}

	INIT_TIME;

	// Sequential execution
	r1 = v;
	BEGIN_TIME;
	seq_oesort(r1);
	END_TIME;
	float usec_seq = usec;
	string sorted = is_sorted(r1.begin(), r1.end()) ? "sorted" : "NOT sorted";
	cout << "Sequential: " << sorted << " in " << usec_seq << " usec" << endl;

	// Parallel execution
	r2 = v;
	BEGIN_TIME;
	par_oesort(r2, nw);
	END_TIME;
	float usec_par = usec;
	sorted = is_sorted(r2.begin(), r2.end()) ? "sorted" : "NOT sorted";
	cout << "Barrier:    " << sorted << " in " << usec_par << " usec" << endl;

	// OpenMP execution
	r3 = v;
	BEGIN_TIME;
	openmp_oesort(r3, nw);
	END_TIME;
	float usec_omp = usec;
	sorted = is_sorted(r3.begin(), r3.end()) ? "sorted" : "NOT sorted";
	cout << "OpenMP:     " << sorted << " in " << usec_omp << " usec\n" << endl;

	// Print results
	cout << "Speedup Barrier:   " << (double)usec_seq / usec_par << endl;
	cout << "Speedup OpenMP:    " << (double)usec_seq / usec_omp << endl;

	return 0;
}