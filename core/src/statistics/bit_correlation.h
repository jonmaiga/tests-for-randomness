#pragma once

#include "chi2.h"
#include "util/bitwise.h"
#include "types.h"

#include <vector>

namespace tfr {

using histogram = std::vector<uint64_t>;
using matrix = std::vector<histogram>;

template <typename T>
matrix bit_count_2d(uint64_t n, stream<T> stream) {
	constexpr auto Size = bit_sizeof<T>() + 1;
	matrix counts(Size, histogram(Size));
	for (uint64_t i = 0; i < n; ++i) {
		const auto x_count = bit_count(stream());
		const auto y_count = bit_count(stream());
		counts[x_count][y_count] += 1;
	}
	return counts;
}

template <typename T>
std::optional<statistic> bit_count_stats(const histogram& bit_counts, double expected_total_count) {
	std::vector<double> expected;
	for (std::size_t i = 0; i < bit_counts.size(); ++i) {
		expected.push_back(expected_total_count * flip_coin_pdf(bit_sizeof<T>(), i));
	}

	const auto merged = merge_bins(bit_counts, expected, 5);
	if (static_cast<double>(merged.observed.size()) /
		static_cast<double>(bit_counts.size()) < 0.35) {
		return {};
	}

	return chi2_stats(merged.observed.size(), to_data(merged.observed), to_data(merged.expected));
}

template <typename T>
sub_test_results bit_count_2d_test(uint64_t n, const stream<T>& stream) {
	const auto counts = bit_count_2d<T>(n, stream);
	sub_test_results results;
	for (int count = 0; count < bit_sizeof<T>(); ++count) {
		const double expected_total_count = n * flip_coin_pdf(bit_sizeof<T>(), count);
		if (const auto stat = bit_count_stats<T>(counts[count], expected_total_count)) {
			results.push_back({ "bit(" + std::to_string(count) + ")", stat });
		}
	}
	return results;
}

}
