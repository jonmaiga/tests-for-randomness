#pragma once

#include <functional>
#include <mutex>
#include <thread>


namespace mixer {

template <typename T>
using job = std::function<T()>;

template <typename T>
using jobs = std::vector<job<T>>;

template <typename T>
void run_jobs(jobs<T> jobs,
              const std::function<void(const T&)>& result_collector,
              unsigned int num_threads) {
	if (jobs.empty()) {
		return;
	}
	const auto job_queue = [&jobs]()-> std::optional<job<T>> {
		static std::mutex m;
		std::lock_guard lg(m);
		if (jobs.empty()) {
			return {};
		}
		auto j = jobs.back();
		jobs.pop_back();
		return j;
	};

	num_threads = std::min(num_threads, static_cast<unsigned int>(jobs.size()));
	std::vector<std::thread> threads;
	for (unsigned int i = 0; i < num_threads; ++i) {
		threads.emplace_back([&job_queue, &result_collector]() {
			while (const auto& job = job_queue()) {
				result_collector((*job)());
			}
		});
	}

	for (auto& t : threads) {
		t.join();
	}
}

}
