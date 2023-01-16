#pragma once

#include "chi2.h"
#include "util/bitwise.h"
#include "types.h"

#include <vector>

namespace mixer {

using histogram = std::vector<uint64_t>;
using matrix = std::vector<histogram>;
using cube = std::vector<matrix>;

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
cube bit_count_3d(uint64_t n, stream<T> stream) {
	constexpr auto Size = bit_sizeof<T>() + 1;
	cube counts(Size, matrix(Size, histogram(Size)));
	for (uint64_t i = 0; i < n; ++i) {
		const auto x_count = bit_count(stream());
		const auto y_count = bit_count(stream());
		const auto z_count = bit_count(stream());
		counts[x_count][y_count][z_count] += 1;
	}
	return counts;
}

template <typename T>
std::optional<statistic> bit_count_stats(const histogram& bit_counts, double expected_total_count, double expected_threshold) {
	return chi2_stats(bit_counts.size(), to_data(bit_counts), [expected_total_count](std::size_t i) {
		return expected_total_count * binomial_pdf(bit_sizeof<T>(), .5, i);
	}, expected_threshold);
}

template <typename T>
sub_test_results bit_count_2d_test(uint64_t n, const stream<T>& stream) {
	const auto counts = bit_count_2d<T>(n, stream);
	sub_test_results results;
	for (int count = 0; count < bit_sizeof<T>(); ++count) {
		const double expected_total_count = n * binomial_pdf(bit_sizeof<T>(), .5, count);
		if (const auto stat = bit_count_stats<T>(counts[count], expected_total_count, 5)) {
			results.push_back({"bit(" + std::to_string(count) + ")", stat});
		}
	}
	return results;
}

template <typename T>
sub_test_results bit_count_3d_test(uint64_t n, const stream<T>& stream) {
	const auto counts = bit_count_3d<T>(n, stream);
	sub_test_results results;
	for (int a = 0; a < counts.size(); ++a) {
		const auto& matrix = counts[a];
		const double pa = binomial_pdf(bit_sizeof<T>(), .5, a);
		for (int b = 0; b < matrix.size(); ++b) {
			const double pb = binomial_pdf(bit_sizeof<T>(), .5, b);
			const double expected_total_count = n * pa * pb;
			if (const auto stat = bit_count_stats<T>(matrix[b], expected_total_count, 25)) {
				results.push_back({"bit(" + std::to_string(a) + "," + std::to_string(b) + ")", stat});
			}
		}
	}
	return results;
}

}
