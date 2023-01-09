#pragma once

#include <vector>
#include "basic.h"

namespace mixer {

struct wald_wolfowitz_data {
	double runs{};
	double n_plus{};
	double n_minus{};
};

template <typename T>
wald_wolfowitz_data generate_wald_wolfowitz_data(const std::vector<T>& data) {
	if (data.empty()) {
		return {};
	}
	auto sorted = data;
	std::sort(sorted.begin(), sorted.end());
	const auto cutoff = get_median(sorted);
	uint64_t n_plus = 0;
	uint64_t n_minus = 0;
	uint64_t runs = 1;
	bool is_current_run_greater = data[0] > cutoff;
	for (const auto v : data) {
		const auto is_greater = v > cutoff;
		if (v != cutoff) {
			is_greater ? ++n_plus : ++n_minus;
		}
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

inline std::optional<statistic> wald_wolfowitz_stats(wald_wolfowitz_data s) {
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
sub_test_results wald_wolfowitz_test(const uint64_t n, stream<T> source) {
	return split_test(source.name, n, 1000000, [&source](uint64_t size) {
		return wald_wolfowitz_stats(generate_wald_wolfowitz_data(get_raw_by_ref(size, source)));
	});
}


}
