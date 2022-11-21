#pragma once

#include <bit>
#include <cstdint>

#include "assertion.h"

namespace mixer {

template <typename T>
T ror(T v, int r) {
	constexpr auto Bits = 8 * sizeof(T);
	return (v >> r) | (v << (Bits - r));
}

template <typename T>
T flip_bit(T i, int bit) {
	assertion(bit >= 0 && bit < 8 * sizeof(T), "bit to flip out of range");
	return i ^ (1ull << bit);
}

template <typename T>
int bit_count(T x) {
	return std::popcount(x);
}

inline uint64_t byte_swap(uint64_t val) {
	return ((((val) & 0xff00000000000000ull) >> 56) |
		(((val) & 0x00ff000000000000ull) >> 40) |
		(((val) & 0x0000ff0000000000ull) >> 24) |
		(((val) & 0x000000ff00000000ull) >> 8) |
		(((val) & 0x00000000ff000000ull) << 8) |
		(((val) & 0x0000000000ff0000ull) << 24) |
		(((val) & 0x000000000000ff00ull) << 40) |
		(((val) & 0x00000000000000ffull) << 56));
}

}
