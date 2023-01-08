#pragma once

#include "chi2.h"
#include "util/bitwise.h"
#include "types.h"

#include <vector>

namespace mixer {

template <typename T>
std::vector<std::vector<uint64_t>> bit_count_correlation_matrix(uint64_t n, stream<T> stream) {
	constexpr auto Size = bit_sizeof<T>() + 1;
	std::vector<std::vector<uint64_t>> counts(Size, std::vector<uint64_t>(Size));
	for (uint64_t i = 0; i < n; ++i) {
		const auto x_count = bit_count(stream());
		const auto y_count = bit_count(stream());
		counts[x_count][y_count] += 1;
	}
	return counts;
}

template <typename T>
std::optional<statistic> bit_count_correlation_matrix_stats(const uint64_t n, const std::vector<uint64_t>& bit_counts, int bit) {
	const double expected_total_count = n * binomial_pdf(bit_sizeof<T>(), .5, bit);
	return chi2_stats(bit_counts.size(), to_data(bit_counts), [expected_total_count](std::size_t i) {
		return expected_total_count * binomial_pdf(bit_sizeof<T>(), .5, i);
	}, 5.);
}

template <typename T>
sub_test_results bit_count_correlation_test(uint64_t n, const stream<T>& stream) {
	const auto counts = bit_count_correlation_matrix<T>(n, stream);
	sub_test_results results;
	for (int bit = 0; bit < bit_sizeof<T>(); ++bit) {
		if (const auto stat = bit_count_correlation_matrix_stats<T>(n, counts[bit], bit)) {
			results.push_back({"bit" + std::to_string(bit), stat});
		}
	}
	return results;
}

}
