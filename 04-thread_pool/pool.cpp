#include <atomic>
#include <chrono>
#include <condition_variable>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <typeinfo>
#include <vector>

using namespace std;

class ThreadPool {
   private:
	int nw = thread::hardware_concurrency();
	bool quit = false;
	mutex mu;
	condition_variable cv;
	vector<thread> pool;
	queue<function<void()>> tasks;

   public:
	ThreadPool(int _nw) : pool(_nw) { run(); };

	void stop() {
		{
			lock_guard<mutex> lock(mu);
			quit = true;
		}
		cv.notify_all();
		// Join all the threads
		for (auto &t : pool) {
			t.join();
		}
		cout << "All threads joined, thread pool stopped" << endl;
	}

	template <typename Func, typename... Args>
	auto submit(Func task, Args... args) {
		using RetType = decltype(task(args...));
		auto prom = make_shared<promise<RetType>>();
		auto weakref = weak_ptr(prom);
		auto t = [this, weakref, task, args...]() {
			auto p = weakref.lock();
			if (p) {
				exec(*p, task, args...);
			}
		};
		{
			lock_guard<mutex> lock(mu);
			tasks.push(move(t));
		}
		cv.notify_one();
		return prom;
	}

   private:
	template <typename Result, typename Func, typename... Args>
	void exec(promise<Result> &p, Func &task, Args &...args) {
		p.set_value(task(args...));
	}
	template <typename Func, typename... Args>
	void exec(promise<void> &p, Func &task, Args &...args) {
		task(args...);
		p.set_value();
	}

	void run() {
		for (auto &t : pool) {
			t = thread([this]() {
				while (true) {
					unique_lock<mutex> lock(mu);
					cv.wait(lock, [&]() { return quit || !tasks.empty(); });
					if (quit) {
						break;
					}
					auto task = move(tasks.front());
					tasks.pop();
					lock.unlock();
					task();
				}
			});
		}
	}
};

int main(int argc, char *argv[]) {
	if (argc < 2) {
		cout << "Usage: " << argv[0] << " nw" << endl;
		return 1;
	}
	int nw = atoi(argv[1]);

	ThreadPool pool(nw);
	cout << "Threads started, submitting tasks" << endl;

	// Submit a task with void -> void
	pool.submit([]() { (cout) << "Thread with void->void" << endl; });

	// Submit a task with args -> void
	pool.submit([](int i) { (cout) << "Thread got " << i << endl; }, 1);

	// Submit a task with args -> return
	auto res = pool.submit(
		[](int i) {
			(cout) << "Thread got " << i << ", returns " << i + 1 << endl;
			return i + 1;
		},
		1);
	auto result = res->get_future().get();
	(cout) << "Main received " << result << endl;

	this_thread::sleep_for(1s);
	cout << "Stopping the pool" << endl;
	pool.stop();

	return 0;
}
