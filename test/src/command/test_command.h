#pragma once

#include "evaluate.h"
#include "source_streams.h"

namespace mixer {

inline auto create_result_callback(int max_power, bool print_intermediate_results = true) {
	return [max_power, print_intermediate_results](const test_battery_result& br) {
		const auto meta = get_meta_analysis(br);
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
std::vector<source<T>> create_seeded_trng(int count) {
	std::vector<source<T>> ts;
	const auto& data = get_trng_data<T>();
	const auto& mix = get_default_mixer<T>();
	for (int i = 1; i <= count; ++i) {
		auto indexer = create_stream_from_mixer(create_counter_stream(mix(i)), mix);
		ts.push_back(
			{create_stream_from_data_by_ref<T>("trng-" + std::to_string(i), data, indexer)}
		);
	}
	return ts;
}

template <typename T>
test_setup<T> create_test_setup(const stream<T> stream) {
	return test_setup<T>{
		stream.name,
		create_seeded_trng<T>(128),
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

	const auto callback = create_result_callback(25, false);
	//test_parallel_multi_pass(callback, create_test_setup<T>(trng_stream));
	for (const auto& m : get_mixers<T>()) { //{mix32::sffs_xmxmx_1}) {
		evaluate_multi_pass(callback, create_test_setup<T>(m));
	}
	write_append(get_config().result_path(), "\n");
}

}
