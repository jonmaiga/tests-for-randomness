#pragma once

#include "combiners32.h"
#include "evaluate.h"
#include "mixers32.h"
#include "mixers64.h"
#include "mixers8.h"
#include "prngs32.h"
#include "prngs64.h"
#include "util/test_setups.h"

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


}
