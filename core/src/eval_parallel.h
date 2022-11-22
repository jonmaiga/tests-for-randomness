#pragma once

#include <mutex>
#include <thread>
#include <vector>

#include "source_streams.h"
#include "test_definitions.h"
#include "util/jobs.h"

namespace mixer {namespace internal {

using test_job_return = std::optional<test_result>;
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
test_jobs create_stream_jobs(const stream_test_definition<T>& test_def, const std::vector<test_factory<T>>& test_factories) {
	test_jobs js;
	for (const auto& factory : test_factories) {
		js.push_back([test_def, factory]()-> test_job_return {
			const auto cfg = factory();
			const auto s = create_stream(cfg);
			const auto& sub_tests = test_def.test(cfg.n, s);
			return test_result{cfg.source.name, cfg.mix.name, test_def.type, sub_tests};
		});
	}
	return js;
}

template <typename T>
test_jobs create_mixer_jobs(const mixer_test_definition<T>& test_def, const std::vector<test_factory<T>>& test_factories) {
	test_jobs js;
	for (const auto& factory : test_factories) {
		if (factory().stream_append_factory) continue;
		js.push_back([test_def, factory]()-> test_job_return {
			const auto cfg = factory();
			const auto& sub_tests = test_def.test(cfg.n, cfg.source, cfg.mix);
			return test_result{cfg.source.name, cfg.mix.name, test_def.type, sub_tests};
		});
	}
	return js;
}

template <typename T>
test_jobs create_test_jobs(const std::vector<test_factory<T>>& test_factories) {
	test_jobs jobs;
	for (const auto& test : get_stream_tests<T>()) {
		append(jobs, create_stream_jobs<T>(test, test_factories));
	}
	for (const auto& test : get_mixer_tests<T>()) {
		append(jobs, create_mixer_jobs<T>(test, test_factories));
	}
	return jobs;
}

template <typename T>
test_battery_result test_rrc_parallel(const mixer<T>& mixer, uint64_t n, unsigned int num_threads) {
	test_battery_result test_result{"rrc", mixer.name};
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

template <typename T>
test_battery_result test_rrc_parallel(const mixer<T>& mixer, uint64_t n) {
	const auto hw_threads = std::thread::hardware_concurrency();
	const auto num_threads = hw_threads > 1 ? hw_threads - 1 : hw_threads;
	return internal::test_rrc_parallel(mixer, n, num_threads);
}

template <typename T>
test_battery_result test_rrc(const mixer<T>& mixer, uint64_t n) {
	return internal::test_rrc_parallel(mixer, n, 1);
}

}
