#pragma once

#include "statistics/chi2.h"
#include "util/algo.h"

namespace tfr {
template <typename T>
std::vector<uint8_t> get_permutation_histogram(const T& data, uint64_t window_size) {
	static_assert(std::is_integral_v<typename T::value_type>);
	std::vector<uint8_t> histogram(1ull << window_size);
	sliding_bit_window(data, window_size, [&histogram](uint64_t v) {
		auto& count = histogram[v];
		if (count < 255) {
			++count;
		}
	});
	return histogram;
}

template <typename T>
std::optional<statistic> permutation_stat(const uint64_t n, const stream<T>& stream, uint64_t permutation_size) {
	const auto total_bits = n * bit_sizeof<T>();
	const auto& histogram = get_permutation_histogram(ranged_stream<T>(stream, n), permutation_size);
	const double expected_count = std::floor(total_bits / permutation_size) / histogram.size();
	return chi2_stats(histogram.size(), to_data(histogram), to_data(expected_count));
}

template <typename T>
uint64_t get_permutation_size(const uint64_t n) {
	// solve n*bits/expected_count = x*2^x, where x is permutation_size
	static const auto log2 = std::log(2);
	const auto total_bits = n * bit_sizeof<T>();
	constexpr double approximated_count_per_bin = 32;
	const double y = static_cast<double>(total_bits) / approximated_count_per_bin;
	return static_cast<uint64_t>(lambert_w_approximation(y * log2) / log2);
}

template <typename T>
sub_test_results permutation_test(uint64_t n, stream<T> source) {
	const auto permutation_size = get_permutation_size<T>(n);
	return {
		{std::to_string(permutation_size), (permutation_stat(n, source, permutation_size))}
	};
}
}
