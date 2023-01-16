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
sub_test_results permutation_test(const uint64_t n, const stream<T>& stream) {
	sub_test_results results;
	for (int permutation_size = 5; permutation_size <= 5; ++permutation_size) {
		const auto histogram = get_permutation_histogram(ranged_stream<T>(stream, n), permutation_size);
		constexpr auto Bits = bit_sizeof<T>();
		const double expected_count = std::floor(Bits * n / permutation_size) / histogram.size();
		results.push_back({"w" + std::to_string(permutation_size), chi2_stats(histogram, expected_count)});
	}
	return results;
}

}
