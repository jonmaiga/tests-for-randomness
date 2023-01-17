#pragma once

#include "combiners32.h"
#include "evaluate.h"
#include "mixers32.h"
#include "mixers64.h"
#include "mixers8.h"
#include "prngs32.h"
#include "prngs64.h"
#include "util/prngutil.h"
#include "util/stream_sources.h"

namespace mixer {

inline auto create_result_callback(int max_power, bool print_intermediate_results = true) {
	return [max_power, print_intermediate_results](const test_battery_result& br) {
		if (br.results.empty()) {
			print_battery_result(br);
			return false;
		}

		const auto meta = get_worst_meta_analysis(br);
		if (!meta) {
			return true;
		}

		const bool proceed = meta->pass() && br.power_of_two() < max_power;
		if (print_intermediate_results || !proceed) {
			print_battery_result(br);
		}
		if (!proceed) {
			std::ostringstream os;
			os << br.test_subject_name << ";" << br.power_of_two() << ";" << (meta->pass() ? "PASS" : "FAIL") << "\n";
			write_append(get_config().result_path() + "result_" + std::to_string(br.bits) + ".txt", os.str());
		}
		return proceed;
	};
}

template <typename T>
streams<T> create_seeded_trng(int sample_count) {
	streams<T> ts;
	const auto& data = get_trng_data<T>();
	const uint64_t interval = data.size() / sample_count;
	for (uint64_t i = 0; i < sample_count; ++i) {
		const auto start_index = i * interval;
		ts.push_back(
			create_stream_from_data_by_ref<T>("trng-" + std::to_string(start_index), data, start_index)
		);
	}
	return ts;
}

template <typename T>
test_setup<T> create_trng_test_setup() {
	return test_setup<T>{
		"trng",
		create_seeded_trng<T>(128),
		all_test_types
	};
}

template <typename T>
test_setup<T> create_combiner_test_setup(combiner<T> combiner) {
	return test_setup<T>{
		combiner.name,
		create_combiner_sources<T>(combiner),
		all_test_types
	};
}

template <typename T>
test_setup<T> create_test_setup(const mixer<T> mixer) {
	return test_setup<T>{
		mixer.name,
		create_sources<T>(),
		all_test_types,
		mixer,
	};
}


inline void test_command() {
	using T = uint32_t;

	const auto callback = create_result_callback(20, false);

	// trng
	evaluate_multi_pass(callback, create_trng_test_setup<T>());

	// mixers
	for (const auto& m : get_mixers<T>()) {
		evaluate_multi_pass(callback, create_test_setup(m));
	}

	// combiners
	for (const auto& combiner : get_combiners<T>()) {
		evaluate_multi_pass(callback, create_combiner_test_setup<T>(combiner));
	}

	// prngs
	for (const auto& prng : get_prngs<T>()) {
		evaluate_multi_pass(callback, create_prng_setup<T>(prng));
	}
}


using per_test_result = std::map<std::string, std::map<std::string, std::string>>;

inline auto create_per_test_callback(per_test_result& result, int max_power, bool print_intermediate_results = true) {
	return [&result, max_power, print_intermediate_results](const test_battery_result& br) {
		if (br.results.empty()) {
			print_battery_result(br);
			return false;
		}
		const auto meta = get_worst_meta_analysis(br);
		if (!meta) {
			return true;
		}

		const bool proceed = meta->pass() && br.power_of_two() < max_power;
		if (print_intermediate_results || !proceed) {
			print_battery_result(br);
		}
		if (!proceed) {
			const auto test_name = get_test_name(br.results.begin()->first.type);
			result[br.test_subject_name][test_name] = std::to_string(br.power_of_two());
		}
		return proceed;
	};
}

template <typename T>
void write(const per_test_result& result) {
	std::stringstream ss;
	ss << ";";
	for (const auto& test : get_tests<T>()) {
		ss << test.name << ";";
	}
	ss << "\n";
	for (const auto& e : result) {
		ss << e.first << ";";
		for (const auto& test : get_tests<T>()) {
			auto it = e.second.find(test.name);
			if (it != e.second.end()) {
				ss << it->second;
			}
			ss << ";";
		}
		ss << "\n";
	}
	write(get_config().result_path() + "per_test.txt", ss.str());
}

inline void per_test() {
	using T = uint32_t;

	per_test_result result;

	const auto callback = create_per_test_callback(result, 20, false);

	for (const auto& test : get_tests<T>()) {
		// trng
		evaluate_multi_pass(callback, create_trng_test_setup<T>().set_tests({test.type}));

		// mixers
		for (const auto& m : get_mixers<T>()) {
			evaluate_multi_pass(callback, create_test_setup(m).set_tests({test.type}));
		}

		// combiners
		for (const auto& combiner : get_combiners<T>()) {
			evaluate_multi_pass(callback, create_combiner_test_setup<T>(combiner).set_tests({test.type}));
		}

		// prngs
		for (const auto& prng : get_prngs<T>()) {
			evaluate_multi_pass(callback, create_prng_setup<T>(prng).set_tests({test.type}));
		}
		write<T>(result);
	}
}

}
