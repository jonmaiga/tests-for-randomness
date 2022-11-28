#pragma once

#include <mutex>
#include <thread>
#include <vector>

#include "source_streams.h"
#include "test_definitions.h"
#include "util/jobs.h"

namespace mixer {

template <typename T>
struct test_setup {
	uint64_t n{};
	mixer<T> mix;
	std::vector<test_factory<T>> source_factories;
	std::vector<test_type> tests;
	unsigned int max_threads = std::max(std::thread::hardware_concurrency() - 4, 2u);
};


namespace internal {

using test_job_return = std::vector<test_result>;
using test_jobs = jobs<test_job_return>;

template <typename T>
stream<T> create_stream(const test_config<T>& cfg) {
	auto s = create_stream_from_mixer<T>(cfg.source, cfg.mix);
	if (cfg.stream_append_factory) {
		return cfg.stream_append_factory(s);
	}
	return s;
}

template <typename T>
test_jobs create_test_jobs(const test_definition<T>& test_def, const std::vector<test_factory<T>>& test_factories) {
	test_jobs js;
	for (const auto& factory : test_factories) {
		js.push_back([test_def, factory]()-> test_job_return {

			std::vector<test_result> results;
			if (const auto& mixer_test = test_def.test_mixer) {
				const auto cfg = factory();
				if (!cfg.stream_append_factory) {
					for (const auto& sub_test : mixer_test(cfg.n, cfg.source, cfg.mix)) {
						if (const auto& stat = sub_test.stats) {
							results.push_back({cfg.source.name, cfg.mix.name, cfg.n, {test_def.type, sub_test.name}, *stat});
						}
					}
				}
			}
			if (const auto& stream_test = test_def.test_stream) {
				const auto cfg = factory();
				const auto s = create_stream(cfg);
				for (const auto& sub_test : stream_test(cfg.n, s)) {
					if (const auto& stat = sub_test.stats) {
						results.push_back(test_result{s.name, cfg.mix.name, cfg.n, {test_def.type, sub_test.name}, *stat});
					}
				}
			}
			return results;
		});
	}
	return js;
}

template <typename T>
test_jobs create_test_jobs(const test_setup<T>& setup) {
	test_jobs jobs;
	for (const auto& test : setup.tests) {
		append(jobs, create_test_jobs<T>(get_test_definition<T>(test), setup.source_factories));
	}
	return jobs;
}

}

template <typename T>
test_battery_result test_parallel(const test_setup<T>& setup) {
	using namespace internal;

	test_battery_result test_result{"test", setup.mix.name, setup.n, setup.source_factories.size()};
	const auto collect_job_results = [&](const test_job_return& results) {
		if (!results.empty()) {
			static std::mutex m;
			std::lock_guard lg(m);
			for (const auto& r : results) {
				test_result.add(r);
			}
		}
	};

	const auto jobs = create_test_jobs(setup);
	run_jobs<test_job_return>(jobs, collect_job_results, setup.max_threads);
	return test_result;
}

}
