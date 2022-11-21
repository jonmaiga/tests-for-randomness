#pragma once

#include <functional>
#include <mutex>
#include <thread>
#include <vector>

#include "source_streams.h"
#include "test_definitions.h"
#include "util/jobs.h"

namespace mixer {namespace internal {

using test_job_return = std::optional<result>;
using test_jobs = jobs<test_job_return>;

inline stream_uint64 create_stream(const test_config& cfg) {
	auto s = create_stream_from_mixer(cfg.source, cfg.mix);
	if (cfg.stream_append_factory) {
		return cfg.stream_append_factory(s);
	}
	return s;
}

template <typename T>
test_jobs create_stream_jobs(const stream_test_definition<T>& test_def, const std::vector<test_factory>& test_factories) {
	test_jobs js;
	for (const auto& factory : test_factories) {
		js.push_back([test_def, factory]()->test_job_return {
			const auto cfg = factory();
			const auto s = create_stream(cfg);
			if (const auto& stat = test_def.test(cfg.n, s)) {
				return result{cfg.source.name, cfg.mix.name, test_def.type, *stat};
			}
			return {};
		});
	}
	return js;
}

inline test_jobs create_mixer_jobs(const mixer_test_definition& test_def, const std::vector<test_factory>& test_factories) {
	test_jobs js;
	for (const auto& factory : test_factories) {
		if (factory().stream_append_factory) continue;
		js.push_back([test_def, factory]()->test_job_return {
			const auto cfg = factory();
			if (const auto& stat = test_def.test(cfg.n, cfg.source, cfg.mix)) {
				return result{cfg.source.name, cfg.mix.name, test_def.type, *stat};
			}
			return {};
		});
	}
	return js;
}

inline test_jobs create_test_jobs(const std::vector<test_factory>& test_factories) {
	test_jobs jobs;
	for (const auto& test : get_stream_tests<uint64_t>()) {
		append(jobs, create_stream_jobs(test, test_factories));
	}
	for (const auto& test : mixer_tests) {
		append(jobs, create_mixer_jobs(test, test_factories));
	}
	return jobs;
}

inline test_result test_rrc_parallel(const mixer64& mixer, uint64_t n, unsigned int num_threads) {
	test_result test_result{"rrc", mixer.name};
	const auto collect_job_results = [&](const test_job_return& result) {
		if (result) {
			static std::mutex m;
			std::lock_guard lg(m);
			test_result.add(*result);
		}
	};

	const auto jobs = create_test_jobs(create_rrc_test_factories(mixer, n));
	run_jobs<test_job_return>(jobs, collect_job_results, num_threads);
	return test_result;
}

}

inline test_result test_rrc_parallel(const mixer64& mixer, uint64_t n) {
	const auto hw_threads = std::thread::hardware_concurrency();
	const auto num_threads = hw_threads > 1 ? hw_threads - 1 : hw_threads;
	return internal::test_rrc_parallel(mixer, n, num_threads);
}

inline test_result test_rrc(const mixer64& mixer, uint64_t n) {
	return internal::test_rrc_parallel(mixer, n, 1);
}

}
