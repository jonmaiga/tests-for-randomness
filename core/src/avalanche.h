#pragma once

#include <algorithm>
#include <cmath>
#include <intrin.h>
#include <optional>

#include "mixers.h"
#include "streams.h"

namespace mixer {

inline uint64_t count_bits_changed(uint64_t a, uint64_t b) {
	const uint64_t ah_bh = a ^ b;
	return __popcnt64(ah_bh);
}

inline uint64_t flip_bit(uint64_t i, int bit) {
	return i ^ (1ull << bit);
}

inline double avalanche_1d_fast_test(const mixer& hash, int n) {
	double mean = 0;
	for (int i = 0; i < n; ++i) {
		const uint64_t h = hash(i);
		for (int bit = 0; bit < 64; ++bit) {
			mean += count_bits_changed(h, hash(flip_bit(i, bit)));
		}
	}
	mean /= 64. * n;
	const double score = 1. - fabs(1. - mean / 32.);
	return score;
}

struct avalanche_stats {
	double max_bias = 0;
	double mean_bias = 0;
	double std_dev_bias = 0;
};

struct avalanche_result {
	std::string stream_name;
	std::string mixer_name;
	uint64_t n{};
	avalanche_stats bic;
	avalanche_stats sac;
};

inline avalanche_result avalanche_bit_independence_test(const stream& stream, const mixer& mixer) {
	uint64_t bic_matrix[64][64] = {{}};
	uint64_t sac_count[64] = {};

	avalanche_result result{stream.name, mixer.name};
	uint64_t n = 0;
	try {
		while (const auto maybeX = stream()) {
			++n;
			const uint64_t x = *maybeX;
			const uint64_t h0 = mixer(x);
			for (int j = 0; j < 64; j++) {
				const uint64_t change = h0 ^ mixer(flip_bit(x, j));
				for (int k = 0; k < 64; k++) {
					const int bit = (change >> k) & 1;
					bic_matrix[j][k] += bit;
					sac_count[k] += bit;
				}
			}
		}
	}
	catch (const std::runtime_error&) {
	}

	result.n = n;
	{
		const double expected_bit_count = n / 2.;
		for (auto& bin : bic_matrix) {
			for (const auto bit_count : bin) {
				const double bias = (bit_count - expected_bit_count) / expected_bit_count;
				result.bic.max_bias = std::max(result.bic.max_bias, fabs(bias));
				result.bic.mean_bias += bias;
				result.bic.std_dev_bias += bias * bias;
			}
		}
		result.bic.mean_bias /= 64. * 64.;
		result.bic.std_dev_bias = sqrt(result.bic.std_dev_bias / (64. * 64. - 1.));
	}

	{
		const double expected_sac_bit_count = 64. * n / 2.;
		for (const auto bit_count : sac_count) {
			const double bias = (bit_count - expected_sac_bit_count) / expected_sac_bit_count;
			result.sac.max_bias = std::max(result.sac.max_bias, fabs(bias));
			result.sac.mean_bias += bias;
			result.sac.std_dev_bias += bias * bias;
		}
		result.sac.mean_bias /= 64.;
		result.sac.std_dev_bias = sqrt(result.sac.std_dev_bias / (64. - 1.));
	}

	return result;
}


}
