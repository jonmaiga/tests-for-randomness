#pragma once

#include "combiners8.h"
#include "combiners16.h"
#include "combiners32.h"
#include "combiners64.h"
#include "evaluate.h"
#include "format_result.h"
#include "mixers8.h"
#include "mixers16.h"
#include "mixers32.h"
#include "mixers64.h"
#include "prngs8.h"
#include "prngs16.h"
#include "prngs32.h"
#include "prngs64.h"
#include "util/test_setups.h"

namespace tfr {
using on_done_callback = std::function<void(test_battery_result, bool pass)>;

inline auto create_result_callback(bool print_intermediate_results = true, const on_done_callback& on_done = {}) {
	return [print_intermediate_results, on_done](const test_battery_result& br, bool is_last) {
		bool proceed = !is_last;
		bool pass = true;
		if (const auto analysis = get_worst_statistic_analysis(br)) {
			if (!is_last) {
				proceed = analysis->pass();
			}
			pass = analysis->pass();
		}

		if (print_intermediate_results || !proceed) {
			print_battery_result(br);
		}
		if (!proceed) {
			if (on_done) {
				on_done(br, pass);
			}
		}
		return proceed;
	};
}

template <typename T>
void test_command() {
	constexpr int max_power_of_two = 25;

	on_done_callback on_done;
	if (!is_debug()) {
		const auto report_filename = get_config().test_result_file_path<T>();
		std::ostringstream os;
		os << "# " << bit_sizeof<T>() << "-bit results\n";
		os << "_While TFR is new you should take the results with a grain of salt._\n\n";
		os << "Tests stop when 2^" << max_power_of_two << " stream elements have been tested. A failed test with '*' suffix indicates the meta analysis failed (non uniform p-values).\n\n";
		os << "Source|TFR|Failures|\n-|-|-|\n";
		write(report_filename, os.str());
		on_done = [report_filename](const test_battery_result& br, bool pass) {
			std::ostringstream os;
			os << escape_for_md(br.test_subject_name) << "|"
				<< (pass ? ">" : "") << br.power_of_two() << "|"
				<< join(get_failed_tests(get_analysis(br)), ", ") << "\n";
			write_append(report_filename, os.str());
		};
	}

	const auto callback = create_result_callback(true, on_done);
	const auto file_ns = "file" + std::to_string(bit_sizeof<T>()) + "::";
	// trng
	if (const auto& data = get_trng_data<T>()) {
		evaluate_multi_pass(callback, create_data_test_setup(file_ns + "trng", *data).range(10, std::min(max_power_of_two, 22)));
	}

	// drng
	if (const auto& data = get_drng_data<T>()) {
		evaluate_multi_pass(callback, create_data_test_setup(file_ns + "drng", *data).range(10, std::min(max_power_of_two, 27)));
	}

	// mixers
	for (const auto& m : get_mixers<T>()) {
		evaluate_multi_pass(callback, create_mixer_test_setup(m).range(10, max_power_of_two));
	}

	// prngs
	for (const auto& prng : get_prngs<T>()) {
		evaluate_multi_pass(callback, create_prng_test_setup<T>(prng).range(10, max_power_of_two));
	}

	// combiners
	for (const auto& combiner : get_combiners<T>()) {
		evaluate_multi_pass(callback, create_combiner_test_setup<T>(combiner).range(10, max_power_of_two));
	}
}
}
