#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>
#include <future>

using namespace std;

#define INIT_TIME auto start = std::chrono::high_resolution_clock::now();\
	auto elapsed = std::chrono::high_resolution_clock::now() - start;\
	auto usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
#define BEGIN_TIME start = std::chrono::high_resolution_clock::now();
#define END_TIME(s,nw) elapsed = std::chrono::high_resolution_clock::now() - start;\
	usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();\
	cout << s << usec << " usecs with " << nw << " threads " << endl;

int f_return(int x) {
	return x;
}

int main(int argc, char *argv[]) {

	int n = atoi(argv[1]);
	int nw = atoi(argv[2]);

	INIT_TIME;

	// Thread overhead calculation
	cout << "Threads" << endl;
	vector<thread*> tid(nw);
	BEGIN_TIME
	for (int i=0; i<n; i++) { //repeat it a given number of times
		//setup thread doing nothing
		for (int j=0; j<nw; j++) {
			tid[j] = new thread([] (int j) { return;}, j);
		}
		//then await all of them
		for (int j=0; j<nw; j++) {
			tid[j]->join();
		}
	}
	END_TIME("raw time threads: ", nw);
	cout << "Average per thread (fork+join) " << ((((float) usec) / ((float) n)) / ((float) nw)) << endl;

	// Async overhead calculation
	cout << "\nAsyncs" << endl;
	vector<future<int>> res(nw);
	BEGIN_TIME
	for (int i=0; i<n; i++) { //repeat it a given number of times
		//setup async returning 1
		for (int j=0; j<nw; j++) {
			res[j] = async(launch::async, f_return, j);
		}
		//then get all the futures
		for (int j=0; j<nw; j++) {
			int r = res[j].get();
		}
	}
	END_TIME("raw time asyncs: ", nw);
	cout << "Average per async (fork+future) " << ((((float) usec) / ((float) n)) / ((float) nw)) << endl;

	return 0;

}