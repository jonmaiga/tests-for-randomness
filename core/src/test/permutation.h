#pragma once

#include "statistics/chi2.h"
#include "util/algo.h"

namespace mixer {

inline std::vector<uint64_t> get_permutation_histogram(const std::vector<uint64_t>& data, int window_size) {
	std::vector<uint64_t> histogram(1ull << window_size);
	sliding_bit_window(data, window_size, window_size, [&histogram](uint64_t v) {
		histogram[v] ++;
	});
	return histogram;
}

inline std::vector<statistic> permutation_test(const uint64_t n, const stream& stream) {
	const auto histogram = get_permutation_histogram(get_raw(n, stream), 5);
	const double expected_count = static_cast<double>(accumulate(histogram)) / histogram.size();
	const auto stats = chi2_stats(histogram, expected_count);
	const auto p_value = chi2_distribution_cdf(stats.chi2, stats.df);
	return {{s_type::permutation, stats.chi2, p_value}};
}


}
