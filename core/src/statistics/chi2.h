#pragma once

#include <vector>

#include "distributions.h"
#include "util/algo.h"
#include "util/math.h"

namespace mixer {

template <typename T>
std::vector<uint64_t> bin_data_for_chi2(const T& data01) {
	static_assert(std::is_floating_point_v<typename T::value_type>);
	std::vector<uint64_t> bins(static_cast<uint64_t>(std::ceil(2 * pow(data01.size(), .4))));
	const auto bin_count = static_cast<double>(bins.size());
	for (const auto v : data01) {
		assertion(is_valid_between_01(v), "Invalid data in chi2");
		auto index = static_cast<std::size_t>(bin_count * v);
		++bins[std::min(bins.size() - 1, index)];
	}
	return bins;
}

inline std::optional<statistic> chi2_stats(std::size_t n, const data_fn& observed, const data_fn& expected, double expected_threshold = 0) {
	double chi2 = 0;
	double df = 0;
	for (std::size_t i = 0; i < n; ++i) {
		const double expected_count = expected(i);
		if (expected_count <= expected_threshold) continue;
		const double diff = observed(i) - expected_count;
		chi2 += diff * diff / expected_count;
		df += 1;
	}
	df = std::max(df - 1, 0.);
	if (is_near(df, 0)) {
		return {};
	}
	const auto p_value = chi2_distribution_cdf(chi2, df);
	return statistic{statistic_type::chi2, chi2, p_value, df};
}

inline std::optional<statistic> chi2_stats(const std::vector<uint64_t>& bins, double expected_count) {
	return chi2_stats(bins.size(), to_data(bins), to_data(expected_count));
}

template <typename T>
std::optional<statistic> chi2_uniform_stats(const T& data01) {
	static_assert(std::is_floating_point_v<typename T::value_type>);
	const auto& bins = bin_data_for_chi2(data01);
	const double expected_count = static_cast<double>(data01.size()) / static_cast<double>(bins.size());
	return chi2_stats(bins, expected_count);
}

template <typename T>
sub_tests chi2_test(const uint64_t n, const stream<T>& stream) {
	return main_sub_test(chi2_uniform_stats(rescale_type_to_01(n, stream)));
}

}
