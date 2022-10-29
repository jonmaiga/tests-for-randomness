#pragma once

#include <vector>
#include "util/algo.h"

namespace mixer {

inline double wald_wolfowitz(const std::vector<uint64_t>& data) {
	auto tmp = data;
	sort(tmp.begin(), tmp.end());
	const auto median = tmp[tmp.size() / 2];

	uint64_t n_plus = 0;
	uint64_t n_minus = 0;
	uint64_t runs = 0;
	bool is_current_run_greater = data[0] > median;
	for (unsigned long long v : data) {
		v > median ? n_plus++ : n_minus++;
		if (v > median != is_current_run_greater) {
			is_current_run_greater = !is_current_run_greater;
			++runs;
		}
	}
	const double n = n_plus + n_minus;
	const double expected_runs_mean = 2. * n_plus * n_minus / n + 1.;
	const double expected_runs_variance = (expected_runs_mean - 1.) * (expected_runs_mean - 2.) / (n - 1.);
	return (runs - expected_runs_mean) / expected_runs_variance;
}

struct wald_wolfowitz_stats {
	double z_score = 0;
};

inline wald_wolfowitz_stats wald_wolfowitz_test(const uint64_t n, const stream& stream) {
	return {wald_wolfowitz(get_raw(n, stream))};
}


}
