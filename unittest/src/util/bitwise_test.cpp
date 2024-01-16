#include <util/bitwise.h>

#include <gtest/gtest.h>

#include "mixers64.h"

namespace tfr {
TEST(bitwise, rol) {
	EXPECT_EQ(rol<uint8_t>(0b10000001, 0), 0b10000001);
	EXPECT_EQ(rol<uint8_t>(0b10000001, 1), 0b00000011);
	EXPECT_EQ(rol<uint8_t>(0b10000001, 2), 0b00000110);
}

TEST(bitwise, ror) {
	EXPECT_EQ(ror<uint8_t>(0b10000001, 0), 0b10000001);
	EXPECT_EQ(ror<uint8_t>(0b10000001, 1), 0b11000000);
	EXPECT_EQ(ror<uint8_t>(0b10000001, 2), 0b01100000);
}

TEST(bitwise, flip_bit) {
	EXPECT_EQ(flip_bit(0, 0), 1);
	EXPECT_EQ(flip_bit(0, 1), 2);
	EXPECT_EQ(flip_bit(0, 2), 4);
	EXPECT_EQ(flip_bit(0, 3), 8);
	EXPECT_EQ(flip_bit(0ull, 63), 9223372036854775808ull);

	EXPECT_EQ(flip_bit(1, 0), 0);
	EXPECT_EQ(flip_bit(1, 1), 3);
	EXPECT_EQ(flip_bit(3, 2), 7);
	EXPECT_EQ(flip_bit(8, 3), 0);
	EXPECT_EQ(flip_bit(1ull, 63), 9223372036854775809ull);
}

TEST(bitwise, is_bit_set) {
	EXPECT_FALSE(is_bit_set(0, 0));
	EXPECT_FALSE(is_bit_set(0xfe, 0));

	EXPECT_FALSE(is_bit_set(2, 0));
	EXPECT_TRUE(is_bit_set(2, 1));
	EXPECT_FALSE(is_bit_set(2, 2));

	using T = uint8_t;
	EXPECT_TRUE(is_bit_set<T>(0b10101011, 0));
	EXPECT_TRUE(is_bit_set<T>(0b10101011, 1));
	EXPECT_FALSE(is_bit_set<T>(0b10101011, 2));
	EXPECT_TRUE(is_bit_set<T>(0b10101011, 3));
	EXPECT_FALSE(is_bit_set<T>(0b10101011, 4));
	EXPECT_TRUE(is_bit_set<T>(0b10101011, 5));
	EXPECT_FALSE(is_bit_set<T>(0b10101011, 6));
	EXPECT_TRUE(is_bit_set<T>(0b10101011, 7));
}


TEST(bitwise, bit_count) {
	EXPECT_EQ(bit_count(0u), 0);
	EXPECT_EQ(bit_count(1u), 1);
	EXPECT_EQ(bit_count(2u), 1);
	EXPECT_EQ(bit_count(3u), 2);
	EXPECT_EQ(bit_count(9223372036854775808ull), 1);
	EXPECT_EQ(bit_count(9223372036854775809ull), 2);
}

TEST(bitwise, reverse_bits_8) {
	EXPECT_EQ(reverse_bits<uint8_t>(0), 0);
	EXPECT_EQ(reverse_bits<uint8_t>(1), 0b10000000);
	EXPECT_EQ(reverse_bits<uint8_t>(3), 0b11000000);
	EXPECT_EQ(reverse_bits<uint8_t>(0b11010010), 0b01001011);
	EXPECT_EQ(reverse_bits<uint8_t>(128), 0b00000001);
}

TEST(bitwise, reverse_bits_16) {
	EXPECT_EQ(reverse_bits<uint16_t>(0), 0);
	EXPECT_EQ(reverse_bits<uint16_t>(1), 0b1000000000000000);
	EXPECT_EQ(reverse_bits<uint16_t>(3), 0b1100000000000000);
	EXPECT_EQ(reverse_bits<uint16_t>(0b1101001000111010), 0b0101110001001011);
}

TEST(bitwise, reverse_bits_32) {
	EXPECT_EQ(reverse_bits<uint32_t>(0), 0);
	EXPECT_EQ(reverse_bits<uint32_t>(1), 0b10000000000000000000000000000000);
	EXPECT_EQ(reverse_bits<uint32_t>(3), 0b11000000000000000000000000000000);
	EXPECT_EQ(reverse_bits<uint32_t>(0b11010010001000010000010000001111), 0b11110000001000001000010001001011);
}

TEST(bitwise, reverse_bits_64) {
	EXPECT_EQ(reverse_bits<uint64_t>(0), 0);
	EXPECT_EQ(reverse_bits<uint64_t>(1), 0b1000000000000000000000000000000000000000000000000000000000000000);
	EXPECT_EQ(reverse_bits<uint64_t>(3), 0b1100000000000000000000000000000000000000000000000000000000000000);
	EXPECT_EQ(reverse_bits<uint64_t>(0b1101001000100001000001000000111100111000110101011111000000101011), 0b1101010000001111101010110001110011110000001000001000010001001011);
}

TEST(bitwise, bit_floor) {
	EXPECT_EQ(bit_floor<uint64_t>(0), 0);
	EXPECT_EQ(bit_floor<uint64_t>(1), 1);
	EXPECT_EQ(bit_floor<uint64_t>(2), 2);
	EXPECT_EQ(bit_floor<uint64_t>(3), 2);
	EXPECT_EQ(bit_floor<uint64_t>(4), 4);
	EXPECT_EQ(bit_floor<uint64_t>(5), 4);
	EXPECT_EQ(bit_floor<uint64_t>(6), 4);
	EXPECT_EQ(bit_floor<uint64_t>(7), 4);
	EXPECT_EQ(bit_floor<uint64_t>(8), 8);
	EXPECT_EQ(bit_floor<uint64_t>(std::numeric_limits<uint64_t>::max()-1), 9223372036854775808u);
}

TEST(bitwise, bit_ceil) {
	EXPECT_EQ(bit_ceil<uint64_t>(0), 0);
	EXPECT_EQ(bit_ceil<uint64_t>(1), 1);
	EXPECT_EQ(bit_ceil<uint64_t>(2), 2);
	EXPECT_EQ(bit_ceil<uint64_t>(3), 4);
	EXPECT_EQ(bit_ceil<uint64_t>(4), 4);
	EXPECT_EQ(bit_ceil<uint64_t>(5), 8);
	EXPECT_EQ(bit_ceil<uint64_t>(6), 8);
	EXPECT_EQ(bit_ceil<uint64_t>(7), 8);
	EXPECT_EQ(bit_ceil<uint64_t>(8), 8);
	EXPECT_EQ(bit_ceil<uint64_t>(std::numeric_limits<uint64_t>::max()-1), 1);
}

template <typename T = uint64_t>
std::vector<uint64_t> sliding_bit_window(const std::vector<T>& data, int window_size) {
	std::vector<uint64_t> r;
	sliding_bit_window(data, window_size, [&r](uint64_t v) { r.push_back(v); });
	return r;
}

TEST(bitwise, sliding_bit_window) {
	using T = std::vector<uint64_t>;
	EXPECT_EQ(sliding_bit_window({ 0 }, 1).size(), 64);
	EXPECT_EQ(sliding_bit_window({ 0 }, 2).size(), 32);
	EXPECT_EQ(sliding_bit_window({ 0 }, 3).size(), 21);
	EXPECT_EQ(sliding_bit_window({ 0 }, 32).size(), 2);
	EXPECT_EQ(sliding_bit_window({ 1 }, 63).size(), 1);

	EXPECT_EQ(sliding_bit_window({ 2 }, 1)[0], 0);
	EXPECT_EQ(sliding_bit_window({ 2 }, 1)[1], 1);
	EXPECT_EQ(sliding_bit_window({ 1ull << 34 | 3 }, 32)[0], 3);
	EXPECT_EQ(sliding_bit_window({ 1ull << 34 | 3 }, 32)[1], 4);
	EXPECT_EQ(sliding_bit_window({ 1 }, 63), (T{ 1 }));
	EXPECT_EQ(sliding_bit_window({ 1ull << 63 }, 63), (T{ 0 }));
}

TEST(bitwise, sliding_bit_window_more) {
	using T = std::vector<uint64_t>;
	EXPECT_EQ(sliding_bit_window({ 0xffffffffffffffff }, 1), (T(64, 1)));
	EXPECT_EQ(sliding_bit_window({ 0xffffffffffffffff }, 2), (T(32, 3)));
	EXPECT_EQ(sliding_bit_window({ 0xffffffffffffffff }, 3), (T(21, 7)));

	{
		auto h = T(64, 1);
		h[0] = 0;
		EXPECT_EQ(sliding_bit_window({ 0xffffffffffffffff - 1 }, 1), h);
	}

	{
		auto expected = T(32, 3);
		expected[expected.size() - 1] = 0;
		const auto actual = sliding_bit_window({0xffffffffffffffff >> 2}, 2);
		EXPECT_EQ(actual, expected);
	}
}

TEST(bitwise, sliding_bit_window_128bits_explicit) {
	uint64_t left = 0b1110010101011011110001011100001000101010001010111110100010110111;
	uint64_t right = 0b1001111011101110001101010111101111110110000110101011111100010001;

	auto r = sliding_bit_window({left, right}, 7);
	EXPECT_EQ(r.size(), 18);
	EXPECT_EQ(r[0], 0b0110111);
	EXPECT_EQ(r[1], 0b1010001);
	EXPECT_EQ(r[8], 0b1100101);
	EXPECT_EQ(r[9], 0b0100011);
	EXPECT_EQ(r[10], 0b1111100);
	EXPECT_EQ(r[11], 0b1010101);
	EXPECT_EQ(r[17], 0b0111101);
}

TEST(bitwise, sliding_bit_window_128bits_explicit_even) {
	uint64_t left = 0b1110010101011011110001011100001000101010001010111110100010110111;
	uint64_t right = 0b1001111011101110001101010111101111110110000110101011111100010001;

	auto r = sliding_bit_window({left, right}, 16);
	EXPECT_EQ(r.size(), 8);
	EXPECT_EQ(r[0], 0b1110100010110111);
	EXPECT_EQ(r[3], 0b1110010101011011);
	EXPECT_EQ(r[4], 0b1011111100010001);
	EXPECT_EQ(r[7], 0b1001111011101110);
}

TEST(bitwise, sliding_window_irl) {
	auto r = sliding_bit_window<uint32_t>({4068143377, 1617203981, 3574511604}, 5);
	EXPECT_EQ(r, (std::vector<uint64_t>{17,8,28,21,7,25,23,1,19,4,25,0,6,26,31,22,14,8,21}));
}

TEST(bitwise, sliding_bit_window_128bits_odd) {
	uint64_t left = 0b1110010101011011110001011100001000101010001010111110100010110111;
	uint64_t right = 0b1001111011101110001101010111101111110110000110101011111100010001;

	auto r = sliding_bit_window({left, right}, 19);
	EXPECT_EQ(r.size(), 6);
	EXPECT_EQ(r[0], 0b0111110100010110111);
	EXPECT_EQ(r[1], 0b0000100010101000101);
	EXPECT_EQ(r[2], 0b1010110111100010111);
	EXPECT_EQ(r[3], 0b1111000100011110010);
	EXPECT_EQ(r[4], 0b1110110000110101011);
	EXPECT_EQ(r[5], 0b1000110101011110111);
}

}
