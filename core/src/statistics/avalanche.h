#pragma once

#include "chi2.h"
#include "util/bitwise.h"
#include "types.h"

namespace tfr {
template <typename T>
std::vector<uint64_t> avalanche_generate_sac(uint64_t n, stream<T> stream, const mixer<T>& mixer) {
	// @attn, using x = stream() directly will make all mixers fail for all counter streams with increments
	// of a power of 2, 1,2,4... I believe this is an error in the test rather than the mixers,
	// maybe the bit flip causes too many duplicates and it becomes biased/too correlated.
	// This happens for +10 rounds of AES and Sha256 as well...
	constexpr auto Bits = bit_sizeof<T>();
	std::vector<uint64_t> sac(Bits + 1);
	for (uint64_t i = 0; i < n; ++i) {
		const T x = mixer(stream());
		const T h0 = mixer(x);
		for (int j = 0; j < Bits; j++) {
			const T change = h0 ^ mixer(flip_bit(x, j));
			++sac[bit_count(change)];
		}
	}
	return sac;
}

template <typename T>
std::vector<uint64_t> avalanche_generate_bic(uint64_t n, stream<T> stream, const mixer<T>& mixer) {
	constexpr auto Bits = bit_sizeof<T>();
	std::vector<uint64_t> bic(Bits * Bits);
	for (uint64_t i = 0; i < n; ++i) {
		const T x = mixer(stream());
		const T h0 = mixer(x);
		for (std::size_t j = 0; j < Bits; j++) {
			const T change = h0 ^ mixer(flip_bit(x, j));
			for (std::size_t k = 0; k < Bits; k++) {
				const int bit = (change >> k) & 1;
				bic[j * Bits + k] += bit;
			}
		}
	}
	return bic;
}

template <typename T>
std::optional<statistic> avalanche_sac_stats(const double n, const std::vector<uint64_t>& bit_counts) {
	constexpr auto Bits = bit_sizeof<T>();
	const double total_count = n * Bits;
	return chi2_stats(bit_counts.size(), to_data(bit_counts), [total_count](std::size_t i) {
		return total_count * flip_coin_pdf(Bits, i);
	}, 5.);
}

inline std::optional<statistic> avalanche_bic_stats(const double n, const std::vector<uint64_t>& bit_counts) {
	return chi2_stats(bit_counts.size(),
	                  mul(to_data(bit_counts), to_data(2)), to_data(n));
}

template <typename T>
sub_test_results avalanche_mixer_sac_test(uint64_t n, const stream<T>& stream, const mixer<T>& mixer) {
	const auto counts = avalanche_generate_sac<T>(n, stream, mixer);
	return main_sub_test(avalanche_sac_stats<T>(n, counts));
}

template <typename T>
sub_test_results avalanche_mixer_bic_test(uint64_t n, const stream<T>& stream, const mixer<T>& mixer) {
	const auto counts = avalanche_generate_bic(n, stream, mixer);
	return main_sub_test(avalanche_bic_stats(n, counts));
}
}
