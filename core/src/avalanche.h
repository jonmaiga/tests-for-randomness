#pragma once

#include <algorithm>
#include <cmath>
#include <optional>

#include "util/bitwise.h"
#include "types.h"

namespace mixer {

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

constexpr uint64_t avalanche_draws = 65;

inline avalanche_result avalanche_test(const uint64_t n, const stream& stream, const mixer& mixer) {
	uint64_t bic_matrix[64][64] = {{}};
	uint64_t sac_count[64] = {};

	avalanche_result result{stream.name, mixer.name};
	uint64_t actual = 0;
	try {
		for (uint64_t i = 0; i < n; ++i) {
			const auto x = stream();
			const uint64_t h0 = mixer(x);
			for (int j = 0; j < 64; j++) {
				const uint64_t change = h0 ^ mixer(flip_bit(x, j));
				for (int k = 0; k < 64; k++) {
					const int bit = (change >> k) & 1;
					bic_matrix[j][k] += bit;
					sac_count[k] += bit;
				}
			}
			++actual;
		}
	}
	catch (const std::runtime_error&) {
	}

	result.n = actual;
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
