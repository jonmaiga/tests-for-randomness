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
	std::vector<source<T>> sources;
	std::vector<test_type> tests;
	unsigned int max_threads = std::max(std::thread::hardware_concurrency() - 4, 2u);
};


namespace internal {

using test_job_return = std::vector<test_result>;
using test_jobs = jobs<test_job_return>;

template <typename T>
stream<T> create_stream(const mixer<T>& mix, const source<T>& cfg) {
	auto s = create_stream_from_mixer<T>(cfg.stream_source, mix);
	if (cfg.stream_append_factory) {
		return cfg.stream_append_factory(s);
	}
	return s;
}

template <typename T>
test_jobs create_test_jobs(
	uint64_t n,
	const mixer<T>& mix,
	const test_definition<T>& test_def,
	const source<T>& source) {
	test_jobs js;

	if (test_def.test_mixer && !source.stream_append_factory) {
		js.push_back([test_def, source, mix, n]()-> test_job_return {
			std::vector<test_result> results;
			for (const auto& sub_test : test_def.test_mixer(n, source.stream_source, mix)) {
				if (const auto& stat = sub_test.stats) {
					results.push_back({source.stream_source.name, mix.name, n, {test_def.type, sub_test.name}, *stat});
				}
			}
			return results;
		});
	}

	if (test_def.test_stream) {
		const auto s = create_stream(mix, source);
		js.push_back([test_def, source = s, mix, n]()-> test_job_return {
			std::vector<test_result> results;
			for (const auto& sub_test : test_def.test_stream(n, source)) {
				if (const auto& stat = sub_test.stats) {
					results.push_back(test_result{source.name, mix.name, n, {test_def.type, sub_test.name}, *stat});
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
		for (const auto& source : setup.sources) {
			append(jobs, create_test_jobs<T>(setup.n, setup.mix, get_test_definition<T>(test), source));
		}
	}
	return jobs;
}

}

template <typename T>
test_battery_result test_parallel(const test_setup<T>& setup) {
	using namespace internal;

	test_battery_result test_result{"test", setup.mix.name, setup.n, setup.sources.size()};
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
