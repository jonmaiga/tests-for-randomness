#include <util/bitwise.h>

#include <gtest/gtest.h>

namespace mixer {

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

TEST(bitwise, bit_count) {
	EXPECT_EQ(bit_count(0u), 0);
	EXPECT_EQ(bit_count(1u), 1);
	EXPECT_EQ(bit_count(2u), 1);
	EXPECT_EQ(bit_count(3u), 2);
	EXPECT_EQ(bit_count(9223372036854775808ull), 1);
	EXPECT_EQ(bit_count(9223372036854775809ull), 2);
}

TEST(bitwise, reverse_bits_32) {
	EXPECT_EQ(reverse_bits(0u), 0);
	EXPECT_EQ(reverse_bits(1u), 0b10000000000000000000000000000000);
	EXPECT_EQ(reverse_bits(3u), 0b11000000000000000000000000000000);
	EXPECT_EQ(reverse_bits(0b11010010001000010000010000001111), 0b11110000001000001000010001001011);
}

TEST(bitwise, reverse_bits_64) {
	EXPECT_EQ(reverse_bits(static_cast<uint64_t>(0)), 0);
	EXPECT_EQ(reverse_bits(static_cast<uint64_t>(1)), 0b1000000000000000000000000000000000000000000000000000000000000000);
	EXPECT_EQ(reverse_bits(static_cast<uint64_t>(3)), 0b1100000000000000000000000000000000000000000000000000000000000000);
	EXPECT_EQ(reverse_bits(0b1101001000100001000001000000111100111000110101011111000000101011), 0b1101010000001111101010110001110011110000001000001000010001001011);
}


}
