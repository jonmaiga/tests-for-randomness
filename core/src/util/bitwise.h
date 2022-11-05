#pragma once


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

}
