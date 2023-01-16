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
			os << br.test_subject_name << ";" << br.power_of_two() << ";" << meta->get_failure_strength() << ";";

			std::string row = br.test_subject_name + ";" + std::to_string(br.power_of_two()) + ";";
			for (const auto& ra : get_result_analysis(br)) {
				if (!ra.analysis.pass()) {
					row += to_string(ra.key) + "(" + std::to_string(ra.analysis.get_failure_strength()) + "), ";
				}
			}
			row += "\n";
			write_append(get_config().result_path() + "result_" + std::to_string(br.bits) + ".txt", row);
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
streams<T> create_seeded_counters(int count) {
	streams<T> ts;
	const auto& mix = get_default_mixer<T>();
	for (int i = 1; i <= count; ++i) {
		ts.push_back(create_counter_stream<T>(1, mix(i)));
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
	//const auto trngs = create_seeded_trng<T>(4 * 2 * bit_sizeof<T>());
	//streams<T> ss;
	//for (size_t i = 0; i < trngs.size(); i += 2) {
	//	ss.push_back(create_combined_stream(trngs[i], trngs[i + 1], combiner));
	//}

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
	//evaluate_multi_pass(callback, create_trng_test_setup<T>());

	// mixers
	// for (const auto& m : get_mixers<T>()) {
	// 	evaluate_multi_pass(callback, create_test_setup(m));
	// }

	// combiners
	for (const auto& combiner : get_combiners<T>()) {
		evaluate_multi_pass(callback, create_combiner_test_setup<T>(combiner));
	}

	// prngs
	// for (const auto& prng : get_prngs<T>()) {
	// 	evaluate_multi_pass(callback, create_prng_setup<T>(prng));
	// }
}

}
