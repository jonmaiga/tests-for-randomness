#pragma once

#include <bit>

namespace mixer {

inline uint64_t ror64(uint64_t v, int r) {
	return (v >> r) | (v << (64 - r));
}

inline uint64_t flip_bit(uint64_t i, int bit) {
	return i ^ (1ull << bit);
}

inline int bit_count(uint64_t x) {
	return std::popcount(x);
}

inline uint64_t byte_swap(uint64_t val) {
	return ((((val) & 0xff00000000000000ull) >> 56) |
                    (((val) & 0x00ff000000000000ull) >> 40) |
                    (((val) & 0x0000ff0000000000ull) >> 24) |
                    (((val) & 0x000000ff00000000ull) >> 8 ) |
                    (((val) & 0x00000000ff000000ull) << 8 ) |
                    (((val) & 0x0000000000ff0000ull) << 24) |
                    (((val) & 0x000000000000ff00ull) << 40) |
                    (((val) & 0x00000000000000ffull) << 56));
}

}
