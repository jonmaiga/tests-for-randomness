#pragma once

#include <cmath>

#include "chi2.h"
#include "util/bitwise.h"
#include "types.h"

namespace mixer {

struct avalanche_stats {
	double score{};
	double chi2{};
	double df{};
};

using avalanche_callback = std::function<void(std::size_t, std::size_t, int)>;

inline void avalanche_generate(uint64_t n,
                               const stream& stream,
                               const mixer& mixer,
                               const avalanche_callback& callback) {

	for (uint64_t i = 0; i < n; ++i) {
		const auto x = stream();
		const uint64_t h0 = mixer(x);
		for (std::size_t j = 0; j < 64; j++) {
			const uint64_t change = h0 ^ mixer(flip_bit(x, j));
			for (std::size_t k = 0; k < 64; k++) {
				const int bit = (change >> k) & 1;
				callback(j, k, bit);
			}
		}
	}
}

inline std::vector<uint64_t> avalanche_generate_sac(uint64_t n, const stream& stream, const mixer& mixer) {
	std::vector<uint64_t> bit_counts(64);
	avalanche_generate(n, stream, mixer, [&bit_counts](std::size_t, std::size_t k, int bit) {
		bit_counts[k] += bit;
	});
	return bit_counts;
}

inline std::vector<uint64_t> avalanche_generate_bic(uint64_t n, const stream& stream, const mixer& mixer) {
	std::vector<uint64_t> bit_counts(64 * 64);
	avalanche_generate(n, stream, mixer, [&bit_counts](std::size_t j, std::size_t k, int bit) {
		bit_counts[j * 64ull + k] += bit;
	});
	return bit_counts;
}

inline avalanche_stats compute_avalanche_stats(const double n, const std::vector<uint64_t>& bit_counts) {
	double score = 0;
	double chi2 = 0;
	constexpr double p = 0.5;
	const double expected_count = n * p;
	const double binomial_to_normal = n * p * (1. - p);
	for (const double bit_count : bit_counts) {
		score += std::abs(2. * static_cast<double>(bit_count) / n - 1.);
		const double diff = (bit_count - expected_count);
		chi2 += diff * diff / binomial_to_normal;
	}
	score = 1. - score / n;
	return {score, chi2, static_cast<double>(bit_counts.size() - 1)};
}

inline std::vector<statistic> avalanche_mixer_test(uint64_t n, const stream& stream, const mixer& mixer) {
	const auto bic_counts = avalanche_generate_bic(n, stream, mixer);
	const auto bic_stats = compute_avalanche_stats(n, bic_counts);
	const auto p_value = chi2_distribution_cdf(bic_stats.chi2, bic_stats.df);
	return {{s_type::avalanche_bic, bic_stats.score, p_value}};
}

}
