#pragma once

#include "statistics/chi2.h"
#include "util/algo.h"

namespace mixer {

template <typename T>
std::vector<uint64_t> get_permutation_histogram(const T& data, int window_size) {
	static_assert(std::is_integral_v<typename T::value_type>);
	std::vector<uint64_t> histogram(1ull << window_size);
	sliding_bit_window(data, window_size, window_size, [&histogram](uint64_t v) {
		histogram[v] ++;
	});
	return histogram;
}

template <typename T>
sub_tests permutation_test(const uint64_t n, const stream<T>& stream) {
	const auto histogram = get_permutation_histogram(get_raw(n, stream), 5);
	const double expected_count = static_cast<double>(accumulate(histogram)) / histogram.size();
	return main_sub_test(chi2_stats(histogram, expected_count));
}


}
