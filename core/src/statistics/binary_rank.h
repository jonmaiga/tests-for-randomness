#pragma once

#include <deque>

#include "chi2.h"
#include "util/bitwise.h"
#include "types.h"

#include <vector>

namespace tfr {
// todo: unify with other matrix
using binary_matrix = std::vector<std::vector<int>>;

inline void swap_rows(binary_matrix& matrix, int row1, int row2) {
	for (size_t i = 0; i < matrix[0].size(); ++i) {
		std::swap(matrix[row1][i], matrix[row2][i]);
	}
}


inline uint64_t calculate_rank(binary_matrix m) {
	const auto rows = m.size();
	const auto cols = m[0].size();
	uint64_t rank = 0;

	for (size_t i = 0; i < rows; ++i) {
		// Find the leftmost non-zero element in the current row
		size_t col = 0;
		while (col < cols && m[i][col] == 0) {
			col++;
		}

		// If a non-zero element is found, increment the rank
		if (col < cols) {
			rank++;

			// Eliminate non-zero elements in the current column in other rows
			for (size_t j = 0; j < rows; ++j) {
				if (j != i && m[j][col] == 1) {
					for (size_t k = 0; k < cols; ++k) {
						m[j][k] = m[j][k] ^ m[i][k];
					}
				}
			}
		}
	}

	return rank;
}

template <typename RangeT>
void for_each_matrix(const RangeT& data, uint64_t matrix_size, const std::function<void(const binary_matrix&)>& callback) {
	binary_matrix tmp_matrix(matrix_size, std::vector(matrix_size, 0));

	for_each_bit(data, [c=0ull, r=0ull, matrix_size, &tmp_matrix, callback](int bit) mutable {
		tmp_matrix[r][c] = bit;
		if (++c == matrix_size) {
			++r;
			c = 0;
		}
		if (c == 0 && r == matrix_size) {
			callback(tmp_matrix);
			r = 0;
		}
	});
}

inline double get_rank_probability(uint64_t matrix_size, uint64_t rank) {
	// from https://eprint.iacr.org/2022/061.pdf 2.1
	const auto n = static_cast<int64_t>(matrix_size);
	const auto r = static_cast<int64_t>(rank);
	const double c = std::pow(2, -(n - r) * (n - r));
	double p = 1.;
	for (int64_t i = 0; i < r; ++i) {
		double numerator = 1. - std::pow(2., i - n);
		numerator *= numerator;
		p *= numerator / (1. - std::pow(2., i - r));
	}
	return c * p;
}

inline std::vector<double> get_rank_probabilities(uint64_t matrix_size) {
	std::deque<double> probabilities;
	uint64_t rank = 0;
	double sum = 0;
	while (rank <= matrix_size) {
		const auto p = get_rank_probability(matrix_size, rank);
		probabilities.push_front(p);
		sum += p;
		++rank;
	}
	assertion(is_near(sum, 1), "Unexpected sum for rank probabilities");
	return {probabilities.begin(), probabilities.end()};
}

template <typename T>
std::optional<statistic> binary_rank_stats(uint64_t n, stream<T> stream, uint64_t matrix_size) {
	const auto ps = get_rank_probabilities(matrix_size);
	std::vector<uint64_t> rank_counts(ps.size(), 0);
	auto matrix_count = 0;
	for_each_matrix(ranged_stream<T>(stream, n), matrix_size,
	                [&rank_counts, matrix_size, &matrix_count](const binary_matrix& m) {
		                const auto r = calculate_rank(m);
		                const auto s = matrix_size - r;
		                rank_counts[s]++;
		                ++matrix_count;
	                });

	return chi2_stats(rank_counts.size(), to_data(rank_counts),
	                  mul(to_data(ps), to_data(matrix_count)), 1.);
}

template <typename T>
sub_test_results binary_rank_test(uint64_t n, const stream<T>& source) {
	return main_sub_test(binary_rank_stats(n, source, 32));
}
}
