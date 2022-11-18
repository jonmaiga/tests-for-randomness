#pragma once

#include <functional>
#include <mutex>
#include <thread>
#include <vector>

#include "source_streams.h"
#include "test_definitions.h"
#include "util/jobs.h"

namespace mixer {namespace internal {

using test_job_return = std::vector<result>;
using test_jobs = jobs<test_job_return>;

inline stream create_stream(const test_config& cfg) {
	auto s = create_stream_from_mixer(cfg.source, cfg.mix);
	if (cfg.stream_append_factory) {
		return cfg.stream_append_factory(s);
	}
	return s;
}

inline test_jobs create_stream_jobs(const stream_test& test, const std::vector<test_factory>& test_factories) {
	test_jobs js;
	for (const auto& factory : test_factories) {
		js.push_back([test, factory]() {
			const auto cfg = factory();
			std::vector<result> results;
			const auto s = create_stream(cfg);
			if (const auto& stat = test(cfg.n, s)) {
				results.push_back({cfg.source.name, cfg.mix.name, *stat});
			}
			return results;
		});
	}
	return js;
}

inline test_jobs create_mixer_jobs(const mixer_test& test, const std::vector<test_factory>& test_factories) {
	test_jobs js;
	for (const auto& factory : test_factories) {
		if (factory().stream_append_factory) continue;
		js.push_back([test, factory]() {
			const auto cfg = factory();
			std::vector<result> results;
			if (const auto& stat = test(cfg.n, cfg.source, cfg.mix)) {
				results.push_back({cfg.source.name, cfg.mix.name, *stat});
			}
			return results;
		});
	}
	return js;
}

inline test_jobs create_test_jobs(const std::vector<test_factory>& test_factories) {
	test_jobs jobs;
	for (const auto& test : stream_tests) {
		append(jobs, create_stream_jobs(test, test_factories));
	}
	for (const auto& test : mixer_tests) {
		append(jobs, create_mixer_jobs(test, test_factories));
	}
	return jobs;
}

inline test_result test_rrc_parallel(const mixer& mixer, uint64_t n, unsigned int num_threads) {
	test_result test_result{"rrc", mixer.name};
	const auto collect_job_results = [&](const test_job_return& results) {
		static std::mutex m;
		std::lock_guard lg(m);
		test_result.add(results);
	};

	const auto jobs = create_test_jobs(create_rrc_test_factories(mixer, n));
	run_jobs<test_job_return>(jobs, collect_job_results, num_threads);
	return test_result;
}

}

inline test_result test_rrc_parallel(const mixer& mixer, uint64_t n) {
	const auto hw_threads = std::thread::hardware_concurrency();
	const auto num_threads = hw_threads > 1 ? hw_threads - 1 : hw_threads;
	return internal::test_rrc_parallel(mixer, n, num_threads);
}

inline test_result test_rrc(const mixer& mixer, uint64_t n) {
	return internal::test_rrc_parallel(mixer, n, 1);
}

}
