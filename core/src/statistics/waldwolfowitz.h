#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

#include "basic.h"
#include "util/algo.h"
#include "distributions.h"
#include "types.h"

namespace mixer {

struct wald_wolfowitz_stats {
	double runs{};
	double n_plus{};
	double n_minus{};
};

inline wald_wolfowitz_stats wald_wolfowitz(const std::vector<uint64_t>& data) {
	if (data.empty()) {
		return {};
	}
	auto tmp = data;
	sort(tmp.begin(), tmp.end());
	const auto median = get_median(tmp);

	uint64_t n_plus = 0;
	uint64_t n_minus = 0;
	uint64_t runs = 1;
	bool is_current_run_greater = data[0] > median;
	for (const auto v : data) {
		if (v != median) {
			v > median ? n_plus++ : n_minus++;
		}
		if (v > median != is_current_run_greater) {
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

inline double wald_wolfowitz_p_value(wald_wolfowitz_stats s) {
	// p-values get an u-shape for trng, suggesting that the variance is greater than expected.
	// info https://support.sas.com/kb/33/092.html
	const double n = s.n_plus + s.n_minus;
	const double expected_runs_mean = 2. * s.n_plus * s.n_minus / n + 1.;
	const double expected_runs_variance = (expected_runs_mean - 1.) * (expected_runs_mean - 2.);
	return z_test(n - 1, s.runs, expected_runs_mean, expected_runs_variance);
}

inline std::vector<statistic> wald_wolfowitz_test(const uint64_t n, const stream& stream) {
	const auto ww = wald_wolfowitz(get_raw(n, stream));
	return {
		{s_type::wald_wolfowitz_runs, ww.runs, wald_wolfowitz_p_value(ww)}
	};
}


}
