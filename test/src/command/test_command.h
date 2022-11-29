#pragma once

#include "eval_parallel.h"
#include "source_streams.h"

namespace mixer {

inline auto create_result_callback(int max_power) {
	return [max_power](const test_battery_result& br) {
		const auto meta = get_meta_analysis(br);
		print_battery_result(br);

		const bool proceed = meta->pass() && br.power_of_two() < max_power;
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
			write_append(get_config().result_path(), row);
		}

		return proceed;
	};
}

template <typename T>
test_setup<T> create_test_setup(const stream<T> stream) {
	return test_setup<T>{
		stream.name,
		{source<T>{stream}},
		all_test_types
	};
}


template <typename T>
test_setup<T> create_test_setup(const mixer<T> mixer) {
	return test_setup<T>{
		mixer.name,
		create_rrc_sources<T>(),
		all_test_types,
		mixer,
	};
}

inline void test_command() {
	using T = uint32_t;
	const auto trng_stream = create_stream_from_data_by_ref<T>("trng", get_trng_data<T>());

	const auto callback = create_result_callback(20);
	test_parallel_multi_pass(callback, create_test_setup<T>(trng_stream));
	for (const auto& m : get_mixers<T>()) {
		test_parallel_multi_pass(callback, create_test_setup<T>(m));
	}
	write_append(get_config().result_path(), "\n");
}

}
