#pragma once

#include "eval_parallel.h"
#include "source_streams.h"

namespace mixer {

template <typename T>
void run_test(const mixer<T>& mixer, const test_callback& callback) {
	const test_setup<T> ts{
		mixer.name,
		create_rrc_sources<T>(),
		all_test_types,
		mixer,
	};
	test_parallel_multi_pass(callback, ts);
}

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

inline void test_command() {
	using T = uint32_t;
	const auto trng_stream = create_stream_from_data_by_ref_thread_safe<T>("trng", get_trng_data<T>());
	const auto trng = create_mixer_from_stream<T>("trng1", trng_stream);

	const auto callback = create_result_callback(20);
	run_test(trng, callback);
	for (const auto& m : get_mixers<T>()) {
		run_test(m, callback);
	}
	write_append(get_config().result_path(), "\n");
}

}
