#include <util/bitwise.h>

#include <gtest/gtest.h>

namespace mixer {

TEST(bitwise, flip_bit) {
	EXPECT_EQ(flip_bit(0, 0), 1);
	EXPECT_EQ(flip_bit(0, 1), 2);
	EXPECT_EQ(flip_bit(0, 2), 4);
	EXPECT_EQ(flip_bit(0, 3), 8);
	EXPECT_EQ(flip_bit(0, 63), 9223372036854775808ull);

	EXPECT_EQ(flip_bit(1, 0), 0);
	EXPECT_EQ(flip_bit(1, 1), 3);
	EXPECT_EQ(flip_bit(3, 2), 7);
	EXPECT_EQ(flip_bit(8, 3), 0);
	EXPECT_EQ(flip_bit(1, 63), 9223372036854775809ull);
}

TEST(bitwise, bit_count) {
	EXPECT_EQ(bit_count(0), 0);
	EXPECT_EQ(bit_count(1), 1);
	EXPECT_EQ(bit_count(2), 1);
	EXPECT_EQ(bit_count(3), 2);
	EXPECT_EQ(bit_count(9223372036854775808ull), 1);
	EXPECT_EQ(bit_count(9223372036854775809ull), 2);
}

}
