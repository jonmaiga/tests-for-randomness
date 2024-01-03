#pragma once

#include <vector>

#include "chi2.h"
#include "types.h"

namespace tfr {
// from: https://github.com/google/paranoid_crypto/blob/main/paranoid_crypto/lib/randomness_tests/nist_suite.py
inline int64_t lfsr_log_probability(uint64_t block_size, uint64_t lfsr_length) {
	assertion(block_size > 0, "Block size must be positive");
	assertion(lfsr_length <= block_size, "Lfsr length must be smaller or equal to block_size");
	if (lfsr_length == 0) {
		return -block_size;
	}
	if (lfsr_length <= block_size / 2) {
		return 2 * lfsr_length - block_size - 1;
	}
	return block_size - 2 * lfsr_length;
}

// adapted from: https://mathematica.stackexchange.com/questions/21132/efficient-implementation-of-a-linear-complexity-measure-of-binary-sequences
inline int berlekamp_massey(const std::vector<int>& u) {
	const auto len = static_cast<int>(u.size());
	std::vector<int> b(len, 0);
	std::vector<int> c(len, 0);
	std::vector<int> b_tmp(len, 0);
	b[0] = c[0] = 1;

	int l = 0;
	int m = 0;

	for (int n = 1; n <= len; ++n) {
		int s = u[n - 1];
		for (int j = 1; j <= l; ++j) {
			s ^= c[j] & u[n - j - 1]; // instead of s += c[j] * u[n-j-1]; s toggles between 0 and 1
		}

		if (s != 0) {
			const int from = n - m;
			const int to = std::min(len, from + l + 1);
			std::copy(cbegin(b), cbegin(b) + l + 1, begin(b_tmp));
			if (2 * l <= n) {
				l = n - l;
				m = n;
				b = c;
			}
			for (int j = from; j < to; ++j) {
				c[j] ^= b_tmp[j - from]; // same as c[j] = (c[j] + b_tmp[j - from]) % 2;
			}
		}
	}
	return l;
}

inline std::vector<double> get_expected_probabilities(uint64_t block_size) {
	std::vector<double> ps;
	auto sum = 0.;
	auto i = 0;
	while (sum < 1.) {
		sum += ps.emplace_back(std::pow(2, lfsr_log_probability(block_size, i)));
		++i;
	}
	return ps;
}

template <typename T>
std::optional<statistic> linear_complexity_stats(uint64_t n, stream<T> stream, uint64_t block_size) {
	const auto ps = get_expected_probabilities(block_size);
	std::vector<uint64_t> counts(ps.size(), 0);
	auto count = 0;
	int log_probability = 0;
	for_each_bit_block(ranged_stream<T>(stream, n), block_size,
	                   [&counts, &count, &log_probability, block_size](const std::vector<int>& bits) {
							const auto l = berlekamp_massey(bits);
							++counts[l];
							++count;
							log_probability += -lfsr_log_probability(block_size, l);
	                   });
	//auto pv = binomial_cdf(log_probability - 1, 0.5, count - 1);
	//return statistic{statistic_type::chi2, static_cast<double>(log_probability), pv, static_cast<double>(count-1)};

	return chi2_stats(counts.size(), to_data(counts),
	                  mul(to_data(ps), to_data(count)), 5.);
}

template <typename T>
uint64_t get_block_size(uint64_t n) {
	constexpr double wanted_blocks = 10. / (1 / 96.);
	constexpr double multiplier = bit_sizeof<T>() / wanted_blocks;
	return std::pow(n * multiplier, 0.6);
}

template <typename T>
sub_test_results linear_complexity_test(uint64_t n, const stream<T>& source) {
	const auto block_size = get_block_size<T>(n);
	if (block_size < 8) {
		return {};
	}
	return {{std::to_string(block_size), linear_complexity_stats(n, source, block_size)}};
}
}
