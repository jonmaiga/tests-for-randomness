#pragma once

#include <cmath>

#include "chi2.h"
#include "util/bitwise.h"
#include "types.h"

namespace mixer {

struct avalanche_stats {
	double chi2{};
	double df{};
};

inline std::vector<uint64_t> avalanche_generate_sac(uint64_t n, const stream& stream, const mixer& mixer) {
	// @attn, using x = stream() directly will make all mixers fail for all counter streams with increments
	// of a power of 2, 1,2,4... I believe this is an error in the test rather than the mixers,
	// maybe the bit flip causes too many duplicates and it becomes biased.
	// This happens for +10 rounds of AES and Sha256 as well...
	std::vector<uint64_t> sac(65);
	for (uint64_t i = 0; i < n; ++i) {
		const auto x = mixer(stream());
		const uint64_t h0 = mixer(x);
		for (int j = 0; j < 64; j++) {
			const auto change = h0 ^ mixer(flip_bit(x, j));
			sac[bit_count(change)] ++;
		}
	}
	return sac;
}

inline std::vector<uint64_t> avalanche_generate_bic(uint64_t n, const stream& stream, const mixer& mixer) {
	std::vector<uint64_t> bic(4096);
	for (uint64_t i = 0; i < n; ++i) {
		const auto x = mixer(stream());
		const uint64_t h0 = mixer(x);
		for (std::size_t j = 0; j < 64; j++) {
			const auto change = h0 ^ mixer(flip_bit(x, j));
			for (std::size_t k = 0; k < 64; k++) {
				const int bit = (change >> k) & 1;
				bic[j * 64ull + k] += bit;
			}
		}
	}
	return bic;
}

inline avalanche_stats compute_avalanche_sac_stats(const double n, const std::vector<uint64_t>& bit_counts) {
	const double total_count = n * 64;
	double chi2 = 0;
	double df = 0;
	for (std::size_t i = 0; i < 65; ++i) {
		const double p = binomial_pdf(64, .5, i);
		const double expected_count = total_count * p;
		if (expected_count < 5) continue;
		const double diff = (bit_counts[i] - expected_count);
		chi2 += diff * diff / expected_count;
		df++;
	}
	return {chi2, df - 1};
}

inline avalanche_stats compute_avalanche_bic_stats(const double n, const std::vector<uint64_t>& bit_counts) {
	double chi2 = 0;
	constexpr double p = 0.5;
	const double expected_count = n * p;
	const double binomial_to_normal = n * p * (1. - p);
	for (const double bit_count : bit_counts) {
		const double diff = (bit_count - expected_count);
		chi2 += diff * diff / binomial_to_normal;
	}
	return {chi2, static_cast<double>(bit_counts.size() - 1)};
}

inline std::vector<statistic> avalanche_mixer_sac_test(uint64_t n, const stream& stream, const mixer& mixer) {
	const auto counts = avalanche_generate_sac(n, stream, mixer);
	const auto stats = compute_avalanche_sac_stats(n, counts);
	const auto p_value = chi2_distribution_cdf(stats.chi2, stats.df);
	return {{s_type::sac, stats.chi2, p_value}};
}

inline std::vector<statistic> avalanche_mixer_bic_test(uint64_t n, const stream& stream, const mixer& mixer) {
	const auto counts = avalanche_generate_bic(n, stream, mixer);
	const auto stats = compute_avalanche_bic_stats(n, counts);
	const auto p_value = chi2_distribution_cdf(stats.chi2, stats.df);
	return {{s_type::bic, stats.chi2, p_value}};
}

}
