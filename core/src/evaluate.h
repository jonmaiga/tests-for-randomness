#pragma once

#include <mutex>
#include <thread>
#include <vector>

#include "test_definitions.h"
#include "util/jobs.h"
#include "util/timer.h"

namespace tfr {

template <typename T>
struct test_setup {
	std::string test_subject_name;
	streams<T> sources;
	std::vector<test_type> tests;
	std::optional<mixer<T>> mix;
	unsigned int max_threads = default_max_threads();
	int start_power_of_two = 10;
	int stop_power_of_two = 20;

	test_setup& set_tests(const std::vector<test_type>& test_types) {
		tests = test_types;
		return *this;
	}

	test_setup& range(int start_power, int stop_power) {
		start_power_of_two = start_power;
		stop_power_of_two = stop_power;
		return *this;
	}
};


namespace internal {

using test_job_return = std::vector<test_result>;
using test_job = job<test_job_return>;
using test_jobs = jobs<test_job_return>;

template <typename T>
stream<T> create_stream(const std::optional<mixer<T>>& mix, const stream<T>& source) {
	return mix ? create_stream_from_mixer<T>(source, *mix) : source;
}

template <typename T>
test_job create_mixer_job(
	uint64_t n,
	const std::string& name,
	const mixer<T>& mix,
	const test_definition<T>& test_def,
	const stream<T>& source) {
	return [test_def, source, mix, name, n]()-> test_job_return {
		std::vector<test_result> results;
		for (const auto& sub_test : test_def.test_mixer(n, source, mix)) {
			if (const auto& stat = sub_test.stats) {
				results.push_back({source.name, name, n, {test_def.type, sub_test.name}, *stat});
			}
		}
		return results;
	};
}

template <typename T>
test_job create_stream_job(uint64_t n, const std::string& name, const test_definition<T>& test_def, const stream<T>& source) {
	return [test_def, source, n, name]()-> test_job_return {
		std::vector<test_result> results;
		for (const auto& sub_test : test_def.test_stream(n, source)) {
			if (const auto& stat = sub_test.stats) {
				results.push_back(test_result{source.name, name, n, {test_def.type, sub_test.name}, *stat});
			}
		}
		return results;
	};
}

template <typename T>
test_jobs create_test_jobs(const uint64_t n, const test_setup<T>& setup) {
	test_jobs jobs;
	const auto& test_subject_name = setup.test_subject_name;
	const auto& mix = setup.mix;
	const auto power_of_two = std::floor(std::log2(n));
	for (const auto& test : setup.tests) {
		const auto& test_def = get_test_definition<T>(test);
		if (power_of_two > test_def.max_power_of_two.value_or(10000)) {
			continue;
		}
		for (const auto& source : setup.sources) {
			if (test_def.test_mixer && mix) {
				// mixer test
				jobs.push_back(create_mixer_job<T>(n, test_subject_name, *mix, test_def, source));
			}
			if (test_def.test_stream) {
				// stream test
				const auto s = create_stream(mix, source);
				jobs.push_back(create_stream_job<T>(n, test_subject_name, test_def, s));
			}
		}
	}
	return jobs;
}

inline auto create_collector(test_battery_result& test_result) {
	return [&](const test_job_return& results) {
		if (!results.empty()) {
			static std::mutex m;
			std::lock_guard lg(m);
			for (const auto& r : results) {
				test_result.add(r);
			}
		}
	};
}

}

template <typename T>
test_battery_result evaluate(uint64_t n, const test_setup<T>& setup) {
	using namespace internal;
	test_battery_result test_result{setup.test_subject_name, n, setup.sources.size(), bit_sizeof<T>()};
	const auto jobs = create_test_jobs(n, setup);
	const timer timer;
	run_jobs<test_job_return>(jobs, create_collector(test_result), setup.max_threads);
	test_result.passed_milliseconds = timer.milliseconds();
	return test_result;
}

using test_callback = std::function<bool(test_battery_result, bool last)>;

template <typename T>
test_battery_result evaluate_multi_pass(const test_callback& result_callback,
                                        const test_setup<T>& setup) {
	assertion(setup.start_power_of_two <= setup.stop_power_of_two, "start stop power of to not valid");
	int power = setup.start_power_of_two;
	test_battery_result result = {};
	while (power <= setup.stop_power_of_two) {
		result = evaluate(1ull << power, setup);
		if (!result_callback(result, power == setup.stop_power_of_two)) {
			return result;
		}
		++power;
	}
	return result;
}

}
