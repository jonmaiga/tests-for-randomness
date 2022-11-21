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

inline uint32_t reverse_bits(uint32_t x) {
	x = ((x >> 1) & 0x55555555) | ((x & 0x55555555) << 1);
	x = ((x >> 2) & 0x33333333) | ((x & 0x33333333) << 2);
	x = ((x >> 4) & 0x0F0F0F0F) | ((x & 0x0F0F0F0F) << 4);
	x = ((x >> 8) & 0x00FF00FF) | ((x & 0x00FF00FF) << 8);
	return (x >> 16) | (x << 16);
}

inline uint64_t reverse_bits(uint64_t x) {
	x = (x & 0xaaaaaaaaaaaaaaaaull) >> 1 | (x & 0x5555555555555555ull) << 1;
	x = (x & 0xccccccccccccccccull) >> 2 | (x & 0x3333333333333333ull) << 2;
	x = (x & 0xf0f0f0f0f0f0f0f0ull) >> 4 | (x & 0x0f0f0f0f0f0f0f0full) << 4;
	x = (x & 0xff00ff00ff00ff00ull) >> 8 | (x & 0x00ff00ff00ff00ffull) << 8;
	x = (x & 0xffff0000ffff0000ull) >> 16 | (x & 0x0000ffff0000ffffull) << 16;
	return x >> 32 | x << 32;
}

}
