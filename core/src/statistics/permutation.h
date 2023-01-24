#pragma once

#include "statistics/chi2.h"
#include "util/algo.h"

namespace mixer {

template <typename T>
std::vector<uint64_t> get_permutation_histogram(const T& data, int window_size) {
	static_assert(std::is_integral_v<typename T::value_type>);
	std::vector<uint64_t> histogram(1ull << window_size);
	sliding_bit_window(data, window_size, [&histogram](uint64_t v) {
		histogram[v] ++;
	});
	return histogram;
}

template <typename T>
std::optional<statistic> permutation_stat(const uint64_t n, const stream<T>& stream) {
	// solve n*bits/expected_count = x*2^x, where x is permutation_size
	static const auto log2 = std::log(2);
	const auto total_bits = n * bit_sizeof<T>();
	constexpr double approximated_count_per_bin = 1000;
	const double y = total_bits / approximated_count_per_bin;
	const int permutation_size = static_cast<int>(lambert_w_approximation(y * log2) / log2);

	const auto histogram = get_permutation_histogram(ranged_stream<T>(stream, n), permutation_size);
	const double expected_count = std::floor(total_bits / permutation_size) / histogram.size();
	return chi2_stats(histogram, expected_count);
}

template <typename T>
sub_test_results permutation_test(const uint64_t n, stream<T> source) {
	return split_test(n, 1000000, [&source](uint64_t size) {
		return permutation_stat(size, source);
	});
}

}
