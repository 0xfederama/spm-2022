#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <functional>
#include <algorithm>
#include <fstream>
#include <ctime>

using namespace std;

#define INIT_TIME auto start = std::chrono::high_resolution_clock::now();\
	auto elapsed = std::chrono::high_resolution_clock::now() - start;\
	auto usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
#define BEGIN_TIME start = std::chrono::high_resolution_clock::now();
#define END_TIME(s) elapsed = std::chrono::high_resolution_clock::now() - start;\
	usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();\
	cout << s << usec << " usecs" << endl;

int CHUNK = 0;
int SCATTER = 1;

// Actively wait for ms milliseconds
void activewait(std::chrono::milliseconds ms) {
	long msecs1 = ms.count();
	auto start1 = std::chrono::high_resolution_clock::now();
	auto end1   = false;
	while(!end1) {
		auto elapsed1 = std::chrono::high_resolution_clock::now() - start1;
		auto msec1 = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed1).count();
		if(msec1>=msecs1)
		end1 = true;
	}
	return;
}

int func(int x) {
	activewait(1ms);
	return x+x;
}

//Map a function over a vector
vector<float> map(vector<float>v, int mode, function<float(float)> f, int pardegree) {
	int len = v.size();
	vector<float> res(len);

	if (mode == CHUNK) {
		//Compute chunks function
		auto compute_chunk = [&](pair<int,int> range) {
			for (int i = range.first; i < range.second; i++) {
				res[i] = f(v[i]);
			}
			return;
		};

		vector<thread> tids;
		vector<pair<int,int>> ranges(pardegree);
		int delta = len / pardegree;
		for (int i = 0; i<pardegree; i++) {
			ranges[i] = make_pair(i*delta, (i != (pardegree-1) ? (i+1)*delta : len));
		}
		for (int i = 0; i < pardegree; i++) {
			tids.push_back(thread(compute_chunk, ranges[i]));
		}
		for (thread& t: tids) {
			t.join();
		}

	} else if (mode == SCATTER) {
		//Compute chunks function
		auto compute_chunk = [&](int t) {
			for (int i = t; i < len; i+=pardegree) {
				res[i] = f(v[i]);
			}
			return;
		};

		vector<thread> tids;
		for (int i = 0; i < pardegree; i++) {
			tids.push_back(thread(compute_chunk, i));
		}
		for (thread& t: tids) {
			t.join();
		}
	}

	return res;
}

int main(int argc, char *argv[]) {

	if (argc < 5) {
		cout << "Usage: " << argv[0] << " seed n nw <mode (0 CHUNK | 1 SCATTER)>" << endl;
		return 1;
	}

	int seed = atoi(argv[1]);
	int n = atoi(argv[2]);
	int nw = atoi(argv[3]);
	int mode = atoi(argv[4]);
	
	const int max = 1024;
	srand(seed);
	vector<float> v(n),r1(n),r2(n);
	// Create a vector of random numbers
	for (int i = 0; i < n; i++) {
		v[i] = rand() % max;
	}

	INIT_TIME;

	//Sequential execution
	BEGIN_TIME;
	transform(v.begin(), v.end(), r1.begin(), func);
	END_TIME("seq: ");

	//Parallel execution
	BEGIN_TIME;
	r2 = map(v, mode, func, nw);
	END_TIME("par: ");

	return 0;

}