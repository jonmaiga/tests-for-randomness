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
	mixer<T> mixer;
	std::vector<test_factory<T>> source_factories;
	std::vector<test_type> tests;
	unsigned int max_threads = std::thread::hardware_concurrency();
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
test_jobs create_stream_jobs(const stream_test_definition<T>& test_def, const std::vector<test_factory<T>>& test_factories) {
	test_jobs js;
	for (const auto& factory : test_factories) {
		js.push_back([test_def, factory]()-> test_job_return {
			const auto cfg = factory();
			const auto s = create_stream(cfg);
			std::vector<test_result> results;
			for (const auto& sub_test : test_def.test(cfg.n, s)) {
				if (const auto& stat = sub_test.stats) {
					results.push_back(test_result{s.name, cfg.mix.name, {test_def.type, sub_test.name}, *stat});
				}
			}
			return results;
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
			std::vector<test_result> results;
			for (const auto& sub_test : test_def.test(cfg.n, cfg.source, cfg.mix)) {
				if (const auto& stat = sub_test.stats) {
					results.push_back({cfg.source.name, cfg.mix.name, {test_def.type, sub_test.name}, *stat});
				}
			}
			return results;
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
test_battery_result test_rrc_parallel(const test_setup<T>& setup) {
	test_battery_result test_result{"rrc", setup.mixer.name};
	const auto collect_job_results = [&](const test_job_return& results) {
		if (!results.empty()) {
			static std::mutex m;
			std::lock_guard lg(m);
			for (const auto& r : results) {
				test_result.add(r);
			}
		}
	};

	const auto jobs = create_test_jobs(create_rrc_test_factories(setup.mixer, setup.n));
	run_jobs<test_job_return>(jobs, collect_job_results, setup.max_threads);
	return test_result;
}

}

template <typename T>
test_battery_result test_rrc_parallel(const test_setup<T>& setup) {
	return internal::test_rrc_parallel(setup);
}

}
