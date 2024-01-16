#pragma once

#include <algorithm>
#include <vector>

#include "distributions.h"
#include "util/algo.h"
#include "util/math.h"

namespace tfr {
inline uint64_t get_bin_count_mathematica(uint64_t n) {
	return static_cast<uint64_t>(std::ceil(2 * pow(n, .4)));
}

template <typename T>
auto create_bin_count_pow2() {
	return [](uint64_t n) {
		const auto mma_bins = get_bin_count_mathematica(n);
		const auto bins_log2 = static_cast<uint64_t>(std::ceil(std::log2(mma_bins)));
		const auto bins_pow2 = static_cast<uint64_t>(1) << bins_log2;
		if constexpr (std::is_same_v<T, uint64_t>) {
			return bins_pow2;
		}
		return std::min(bins_pow2, static_cast<uint64_t>(std::numeric_limits<T>::max()) + 1);
	};
}

using bin_count_function = std::function<uint64_t(uint64_t)>;

template <typename T>
std::vector<uint64_t> bin_data_for_chi2(const T& data, const bin_count_function& get_bin_count) {
	std::vector<uint64_t> bins(get_bin_count(data.size()));
	const auto bin_count = static_cast<double>(bins.size());
	for (const auto vv : data) {
		const auto v = rescale_type_to_01(vv);
		assertion(is_valid_between_01(v), "Invalid data in chi2");
		auto index = static_cast<std::size_t>(bin_count * v);
		++bins[std::min(bins.size() - 1, index)];
	}
	return bins;
}

template <typename ObservedT, typename ExpectedT>
std::optional<statistic> chi2_stats(std::size_t n, const ExpectedT& observed, const ObservedT& expected, double expected_threshold = 0) {
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

inline std::optional<statistic> chi2_stats(const std::vector<uint64_t>& bins, double expected_count, double expected_threshold = 0) {
	return chi2_stats(bins.size(), to_data(bins), to_data(expected_count), expected_threshold);
}

template <typename RangeT>
std::optional<statistic> chi2_uniform_stats(const RangeT& data, const bin_count_function& bin_counter) {
	const auto& bins = bin_data_for_chi2(data, bin_counter);
	const double expected_count = static_cast<double>(data.size()) / static_cast<double>(bins.size());
	return chi2_stats(bins, expected_count);
}
}
