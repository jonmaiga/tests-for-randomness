#pragma once

#include <vector>
#include "basic.h"

namespace mixer {

struct wald_wolfowitz_statistics {
	double runs{};
	double n_plus{};
	double n_minus{};
};

template<typename T>
wald_wolfowitz_statistics wald_wolfowitz_stats(const std::vector<T>& data) {
	if (data.empty()) {
		return {};
	}
	const auto cutoff = get_mean(data);
	uint64_t n_plus = 0;
	uint64_t n_minus = 0;
	uint64_t runs = 1;
	bool is_current_run_greater = data[0] > cutoff;
	for (const auto v : data) {
		const auto is_greater = v > cutoff;
		if (v != cutoff) {
			is_greater ? n_plus++ : n_minus++;
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

inline double wald_wolfowitz_p_value(wald_wolfowitz_statistics s) {
	// info https://support.sas.com/kb/33/092.html
	const double n = s.n_plus + s.n_minus;
	if (is_near(n, 0)) {
		return 0;
	}
	const double expected_runs_mean = 2. * s.n_plus * s.n_minus / n + 1.;
	const double expected_runs_variance = (expected_runs_mean - 1.) * (expected_runs_mean - 2.);
	return z_test(n - 1, s.runs, expected_runs_mean, expected_runs_variance);
}

inline std::vector<statistic> wald_wolfowitz_test(const uint64_t n, const stream& stream) {
	const auto ww = wald_wolfowitz_stats(get_raw(n, stream));
	return {
		{s_type::wald_wolfowitz_runs, ww.runs, wald_wolfowitz_p_value(ww)}
	};
}


}
