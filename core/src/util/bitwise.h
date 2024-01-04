#pragma once

#include <bit>
#include <cstdint>
#include <vector>

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
	assertion(r >= 0 && r < Bits, "ror out of range, should be in range 0 < r < bits in type");
	return (v << r) | (v >> (Bits - r));
}

template <typename T>
T rol(T v, int r) {
	constexpr auto Bits = bit_sizeof<T>();
	assertion(r >= 0 && r < Bits, "rol out of range, should be in range 0 < r < bits in type");
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

template<typename T>
T bit_width(T x) {
	return std::numeric_limits<T>::digits - std::countl_zero(x);
}

template<typename T>
T bit_floor(T x) {
	if (x != 0)
        return T{1} << (bit_width(x) - 1);
    return 0;
}

template<typename T>
T bit_ceil(T x) {
	if (x != 0) {
		return T(1) << bit_width(T(x - 1));
	}
    return 0;
}

template <typename RangeT, typename CallbackT>
void for_each_bit(const RangeT& data, CallbackT callback) {
	constexpr auto Size = bit_sizeof<typename RangeT::value_type>();
	for (const auto v : data) {
		for (int b = 0; b < Size; ++b) {
			callback(is_bit_set(v, b));
		}
	}
}

template <typename RangeT, typename CallbackT>
void for_each_bit_block(const RangeT& data, uint64_t block_size, CallbackT callback) {
	auto block = std::vector<int>(block_size, 0);
	for_each_bit(data, [i=0ull, &block, callback](bool bit) mutable {
		block[i] = bit;
		if (++i == block.size()) {
			callback(block);
			i = 0;
		}
	});
}

template <typename T, typename CallbackT>
void sliding_bit_window(const T& data, int window_size, CallbackT callback) {
	assertion(window_size > 0 && window_size < 64, "Invalid window size");
	uint64_t v = 0;
	int c = 0;

	const auto acc = [callback, c, v, window_size](bool is_set) mutable {
		v |= (is_set ? 1ull : 0ull) << c;
		++c;
		if (c == window_size) {
			callback(v);
			c = 0;
			v = 0;
		}
	};

	for_each_bit(data, acc);
}

template<class To, class From>
std::enable_if_t<
    sizeof(To) == sizeof(From) &&
    std::is_trivially_copyable_v<From> &&
    std::is_trivially_copyable_v<To>,
    To>
bit_cast(const From& src) noexcept {
    static_assert(std::is_trivially_constructible_v<To>,
        "This implementation additionally requires "
        "destination type to be trivially constructible");
    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}

template<typename T>
constexpr T byteswap(T value) noexcept {
    static_assert(std::has_unique_object_representations_v<T>, "T may not have padding bits");
    auto value_representation = bit_cast<std::array<std::byte, sizeof(T)>>(value);
    std::reverse(value_representation.begin(), value_representation.end());
    return bit_cast<T>(value_representation);
}

}
