#pragma once

#include <intrin.h>

namespace mixer {

inline uint64_t ror64(uint64_t v, int r) {
	return (v >> r) | (v << (64 - r));
}

inline uint64_t flip_bit(uint64_t i, int bit) {
	return i ^ (1ull << bit);
}

inline uint64_t count_bits_changed(uint64_t a, uint64_t b) {
	const uint64_t ah_bh = a ^ b;
	return __popcnt64(ah_bh);
}

}
