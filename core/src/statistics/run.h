#pragma once

#include <vector>
#include "basic.h"

namespace tfr {
struct runs_data {
	double runs{};
	double n_plus{};
	double n_minus{};
};

template <typename RangeT>
runs_data generate_runs_data(const RangeT& data, const typename RangeT::value_type cutoff) {
	if (data.empty()) {
		return {};
	}
	uint64_t n_plus = 0;
	uint64_t n_minus = 0;
	uint64_t runs = 1;
	bool is_current_run_greater = *data.begin() > cutoff;
	for (const auto v : data) {
		if (v == cutoff) {
			continue;
		}
		const auto is_greater = v > cutoff;
		is_greater ? ++n_plus : ++n_minus;

		if (is_greater != is_current_run_greater) {
			is_current_run_greater = !is_current_run_greater;
			++runs;
		}
	}
	return {
		static_cast<double>(runs),
		static_cast<double>(n_plus),
		static_cast<double>(n_minus)
	};
}

inline std::optional<statistic> runs_stats(runs_data s) {
	// https://en.wikipedia.org/wiki/Wald%E2%80%93Wolfowitz_runs_test
	// info https://support.sas.com/kb/33/092.html
	// exact, z-test, improved: https://ncss-wpengine.netdna-ssl.com/wp-content/themes/ncss/pdf/Procedures/NCSS/Analysis_of_Runs.pdf
	const double n = s.n_plus + s.n_minus;
	if (is_near(n, 0)) {
		return {};
	}
	const double expected_runs_mean = 2. * s.n_plus * s.n_minus / n + 1.;
	const double expected_runs_variance = (expected_runs_mean - 1.) * (expected_runs_mean - 2.);
	return z_test(n, s.runs, expected_runs_mean, expected_runs_variance);
}

template <typename T>
sub_test_results runs_test(const uint64_t n, stream<T> source) {
	return main_sub_test(runs_stats(
		generate_runs_data(ranged_stream(source, n), std::numeric_limits<T>::max() / 2)
	));
}
}
