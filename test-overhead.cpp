#include <barrier>
#include <iostream>
#include <thread>
#include <vector>

#define INIT_TIME                                                   \
	auto start = std::chrono::high_resolution_clock::now();         \
	auto end   = std::chrono::high_resolution_clock::now() - start; \
	auto usec =                                                     \
		std::chrono::duration_cast<std::chrono::microseconds>(end).count();
#define BEGIN_TIME start = std::chrono::high_resolution_clock::now();
#define END_TIME                                              \
	end	 = std::chrono::high_resolution_clock::now() - start; \
	usec = std::chrono::duration_cast<std::chrono::microseconds>(end).count();

float get_overhead(int nw) {
	// Comment lines 21-25 to measure only T_split+T_merge
	INIT_TIME;
	float overhead = 0;
	std::vector<std::thread> threads(nw);
	std::barrier sync_point(nw, [&]() { return; });
	BEGIN_TIME;
	for (int j = 0; j < nw; j++) {
		threads[j] = std::thread([&]() { 
			sync_point.arrive_and_wait(); 
			//return;
		});
	}
	for (int j = 0; j < nw; j++) {
		threads[j].join();
	}
	END_TIME;
	return float(usec / nw);
}

int main(int argc, char *argv[]) {
	int n	  = 1000;
	int nw	  = 1;
	float avg = 0;
	for (int i = 0; i < n; i++) {
		avg += get_overhead(nw);
	}
	std::cout << "Average overhead: " << avg / n << " usec" << std::endl;
	return 0;
}