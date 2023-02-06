#pragma once

#include <bit>
#include <cstdint>

#include "assertion.h"

namespace tfr {

template <typename T>
constexpr int bit_sizeof() {
	return 8 * sizeof(T);
}

template <typename T>
constexpr int shift_sizeof() {
	if constexpr (std::is_same_v<T, uint64_t>) return 6;
	if constexpr (std::is_same_v<T, uint32_t>) return 5;
	if constexpr (std::is_same_v<T, uint16_t>) return 4;
	if constexpr (std::is_same_v<T, uint8_t>) return 3;
	else {
		//static_assert(sizeof(T) == 0, "T not supported");
		assertion(false, "T not supported");
	}
	return 0;
}

template <typename T>
T ror(T v, int r) {
	constexpr auto Bits = bit_sizeof<T>();
	return (v << r) | (v >> (Bits - r));
}

template <typename T>
T rol(T v, int r) {
	constexpr auto Bits = bit_sizeof<T>();
	return (v >> r) | (v << (Bits - r));
}

template <typename T>
T flip_bit(T i, int bit) {
	assertion(bit >= 0 && bit < bit_sizeof<T>(), "bit to flip out of range");
	return i ^ (1ull << bit);
}

template <typename T>
bool is_bit_set(T i, int bit) {
	assertion(bit >= 0 && bit < bit_sizeof<T>(), "is bit set out of range");
	return ((i >> bit) & 1) != 0;
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

template <typename T> T reverse_bits(T x);

template <>
inline uint8_t reverse_bits(uint8_t x) {
	constexpr uint8_t lookup[16] = {0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe, 0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf};
	return (lookup[x & 0b1111] << 4) | lookup[x >> 4];
}

template <>
inline uint16_t reverse_bits(uint16_t x) {
	x = ((x >> 1) & 0x5555) | ((x & 0x5555) << 1);
	x = ((x >> 2) & 0x3333) | ((x & 0x3333) << 2);
	x = ((x >> 4) & 0x0F0F) | ((x & 0x0F0F) << 4);
	return (x >> 8) | (x << 8);
}


template <>
inline uint32_t reverse_bits(uint32_t x) {
	x = ((x >> 1) & 0x55555555) | ((x & 0x55555555) << 1);
	x = ((x >> 2) & 0x33333333) | ((x & 0x33333333) << 2);
	x = ((x >> 4) & 0x0F0F0F0F) | ((x & 0x0F0F0F0F) << 4);
	x = ((x >> 8) & 0x00FF00FF) | ((x & 0x00FF00FF) << 8);
	return (x >> 16) | (x << 16);
}

template <>
inline uint64_t reverse_bits(uint64_t x) {
	x = (x & 0xaaaaaaaaaaaaaaaaull) >> 1 | (x & 0x5555555555555555ull) << 1;
	x = (x & 0xccccccccccccccccull) >> 2 | (x & 0x3333333333333333ull) << 2;
	x = (x & 0xf0f0f0f0f0f0f0f0ull) >> 4 | (x & 0x0f0f0f0f0f0f0f0full) << 4;
	x = (x & 0xff00ff00ff00ff00ull) >> 8 | (x & 0x00ff00ff00ff00ffull) << 8;
	x = (x & 0xffff0000ffff0000ull) >> 16 | (x & 0x0000ffff0000ffffull) << 16;
	return x >> 32 | x << 32;
}

}
