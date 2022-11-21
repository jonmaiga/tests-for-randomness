#pragma once

#include "chi2.h"
#include "util/bitwise.h"
#include "types.h"

namespace mixer {

template <typename T>
std::vector<uint64_t> avalanche_generate_sac(uint64_t n, stream<T> stream, const mixer<T>& mixer) {
	// @attn, using x = stream() directly will make all mixers fail for all counter streams with increments
	// of a power of 2, 1,2,4... I believe this is an error in the test rather than the mixers,
	// maybe the bit flip causes too many duplicates and it becomes biased.
	// This happens for +10 rounds of AES and Sha256 as well...
	constexpr auto Size = 8 * sizeof(T);
	std::vector<uint64_t> sac(Size);
	for (uint64_t i = 0; i < n; ++i) {
		const auto x = mixer(stream());
		const auto h0 = mixer(x);
		for (int j = 0; j < Size; j++) {
			const auto change = h0 ^ mixer(flip_bit(x, j));
			++sac[std::popcount(change)];
		}
	}
	return sac;
}

template <typename T>
std::vector<uint64_t> avalanche_generate_bic(uint64_t n, stream<T> stream, const mixer<T>& mixer) {
	constexpr auto Size = 8 * sizeof(T);
	std::vector<uint64_t> bic(Size * Size);
	for (uint64_t i = 0; i < n; ++i) {
		const auto x = mixer(stream());
		const auto h0 = mixer(x);
		for (std::size_t j = 0; j < Size; j++) {
			const auto change = h0 ^ mixer(flip_bit(x, j));
			for (std::size_t k = 0; k < Size; k++) {
				const int bit = (change >> k) & 1;
				bic[j * Size + k] += bit;
			}
		}
	}
	return bic;
}

inline std::optional<statistic> avalanche_sac_stats(const double n, const std::vector<uint64_t>& bit_counts) {
	const double total_count = n * 64;
	return chi2_stats(bit_counts.size(), to_data(bit_counts), [total_count](std::size_t i) {
		return total_count * binomial_pdf(64, .5, i);
	}, 5.);
}

inline std::optional<statistic> avalanche_bic_stats(const double n, const std::vector<uint64_t>& bit_counts) {
	return chi2_stats(bit_counts.size(),
	                  mul(to_data(bit_counts), to_data(2)), to_data(n));
}

template <typename T>
std::optional<statistic> avalanche_mixer_sac_test(uint64_t n, const stream<T>& stream, const mixer<T>& mixer) {
	const auto counts = avalanche_generate_sac(n, stream, mixer);
	return avalanche_sac_stats(n, counts);
}

template <typename T>
std::optional<statistic> avalanche_mixer_bic_test(uint64_t n, const stream<T>& stream, const mixer<T>& mixer) {
	const auto counts = avalanche_generate_bic(n, stream, mixer);
	return avalanche_bic_stats(n, counts);
}

}
