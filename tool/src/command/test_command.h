#pragma once

#include "combiners32.h"
#include "evaluate.h"
#include "mixers32.h"
#include "mixers64.h"
#include "mixers8.h"
#include "prngs32.h"
#include "prngs64.h"
#include "util/test_setups.h"

namespace tfr {

inline auto create_result_callback(bool print_intermediate_results = true) {
	return [print_intermediate_results](const test_battery_result& br, bool is_last) {
		bool proceed = !is_last;
		bool pass = true;
		if (proceed) {
			if (const auto analysis = get_worst_statistic_analysis(br)) {
				proceed = analysis->pass();
				pass = analysis->pass();
			}
		}
		if (print_intermediate_results || !proceed) {
			print_battery_result(br);
		}
		if (!proceed) {
			std::ostringstream os;
			os << br.test_subject_name << ";" << br.power_of_two() << ";" << (pass ? "PASS" : "FAIL") << "\n";
			write_append(get_config().result_dir() + "result_" + std::to_string(br.bits) + ".txt", os.str());
		}
		return proceed;
	};
}

inline void test_command() {
	using T = uint32_t;

	const auto callback = create_result_callback(true);

	// trng
	if (const auto* data = get_trng_data<T>()) {
		evaluate_multi_pass(callback, create_data_test_setup<T>("trng", *data));
	}

	// drng
	if (const auto* data = get_drng_data<T>()) {
		evaluate_multi_pass(callback, create_data_test_setup<T>("drng", *data));
	}

	// mixers
	for (const auto& m : get_mixers<T>()) {
		evaluate_multi_pass(callback, create_mixer_test_setup(m));
	}

	// combiners
	for (const auto& combiner : get_combiners<T>()) {
		evaluate_multi_pass(callback, create_combiner_test_setup<T>(combiner));
	}

	// prngs
	for (const auto& prng : get_prngs<T>()) {
		evaluate_multi_pass(callback, create_prng_test_setup<T>(prng));
	}
}


}
