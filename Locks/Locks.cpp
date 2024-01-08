#include <iostream>
#include <atomic>
#include <algorithm>
#include <random>
#include <vector>
#include <chrono>
#include <fstream>
#include <thread>
#include <numeric>
#include <iterator>

class TASLock {
public:
	TASLock() : locked(false) {}

	void lock() {
		int backoff = 1;

		while (locked.exchange(true, std::memory_order_acquire)) {
			backoff *= 2;
			backoff = std::min(backoff, MAX_BACKOFF);

			for (int i = 0; i < backoff; ++i) {
				pause();
			}
		}
	}

	void unlock() {
		locked.store(false, std::memory_order_release);
	}

private:
	std::atomic<bool> locked;
	const int MAX_BACKOFF = 64;

	void pause() {
		std::this_thread::yield();
	}
};

class TTASLock {
public:
	TTASLock() : locked(false) {}

	void lock() {
		int backoff = 1;

		while (true) {
			while (locked.load(std::memory_order_relaxed)) {
				backoff *= 2;
				backoff = std::min(backoff, MAX_BACKOFF);

				for (int i = 0; i < backoff; ++i) {
					pause();
				}
			}

			if (!locked.exchange(true, std::memory_order_acquire)) {
				return;
			}
		}
	}

	void unlock() {
		locked.store(false, std::memory_order_release);
	}

private:
	std::atomic<bool> locked;
	const int MAX_BACKOFF = 64;

	void pause() {
		std::this_thread::yield();
	}
};

class TicketLock {
public:
	TicketLock() : next_ticket(0), now_serving(0) {}

	void lock() {
		const int my_ticket = next_ticket.fetch_add(1, std::memory_order_relaxed);

		int backoff = 1;

		while (my_ticket != now_serving.load(std::memory_order_acquire)) {
			backoff *= 2;
			backoff = std::min(backoff, MAX_BACKOFF);

			for (int i = 0; i < backoff; ++i) {
				pause();
			}
		}
	}

	void unlock() {
		now_serving.fetch_add(1, std::memory_order_release);
	}

private:
	std::atomic<int> next_ticket;
	std::atomic<int> now_serving;
	const int MAX_BACKOFF = 64;

	void pause() {
		std::this_thread::yield();
	}
};


int main() {
	    TASLock lock;
	    //TTASLock lock;
	//TicketLock lock;
	int max_nums_threads = 16;
	std::vector<double> avg_times;
	std::vector<double> max_times;

	for (int num_threads = 1; num_threads <= max_nums_threads; ++num_threads) {
		std::vector<std::thread> threads;
		std::vector<double> times;

		for (int i = 0; i < num_threads; ++i) {
			threads.emplace_back([&lock, &times](){
                for (int j = 0; j < 1000; ++j) {
                    auto begin = std::chrono::steady_clock::now();
                    lock.lock();
                    auto end = std::chrono::steady_clock::now();
                    times.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
                    lock.unlock();
                }
            });
		}

		for (int i = 0; i < num_threads; ++i) {
			threads[i].join();
		}

		avg_times.push_back(static_cast<double>(std::accumulate(times.begin(), times.end(), 0)) / times.size());
		max_times.push_back(static_cast<double>(*std::max_element(times.begin(), times.end())));
	}
	
	std::cout << "=====TTAS======\n";
	std::cout << "Average times: ";
	std::copy(avg_times.begin(), avg_times.end(), std::ostream_iterator<double>(std::cout, ", "));
	std::cout << '\n';
	std::cout << "Maximuum times: ";
	std::copy(max_times.begin(), max_times.end(), std::ostream_iterator<double>(std::cout, ", "));

}	