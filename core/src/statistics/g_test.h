#pragma once

#include <algorithm>
#include <vector>

#include "distributions.h"
#include "util/algo.h"
#include "util/math.h"

namespace tfr {
template <typename FunctionT1, typename FunctionT2>
std::optional<statistic> g_test_stats(std::size_t n, const FunctionT1& observed, const FunctionT2& expected, double expected_threshold = 1) {
	assertion(expected_threshold > 0, "G test requires expected threshold > 0");
	double g = 0;
	double total_observed = 0;
	double total_expected = 0;
	double df = 0;
	for (std::size_t i = 0; i < n; ++i) {
		const double expected_count = expected(i);
		if (expected_count < expected_threshold) continue;
		const auto observed_count = observed(i);
		if (observed_count <= 1e-17) continue;

		g += observed_count * std::log(observed_count / expected_count);
		total_observed += observed_count;
		total_expected += expected_count;
		df += 1;
	}
	df = std::max(df - 1, 0.);
	if (is_near(df, 0)) {
		return {};
	}
	g -= total_observed * std::log(total_observed / total_expected);
	assertion(g >= 0, "The g test statistic is negative, make sure that total observed == total expected.");
	g *= 2;
	const auto p_value = chi2_distribution_cdf(g, df);
	return statistic{statistic_type::chi2, g, p_value, df};
}


inline std::optional<statistic> g_test_stats(const std::vector<uint64_t>& bins, double expected_count, double expected_threshold = 1) {
	return g_test_stats(bins.size(), to_data(bins), to_data(expected_count), expected_threshold);
}
}
