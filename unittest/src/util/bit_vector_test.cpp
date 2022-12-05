#include <util/bitvector.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace mixer {

TEST(bit_vector, basic) {
	bit_vector b;
	EXPECT_EQ(b.count(), 0);
	EXPECT_EQ(b.size(), 0);

	b.add(0, 1);
	EXPECT_EQ(b.count(), 0);
	EXPECT_EQ(b.size(), 1);
	EXPECT_EQ(b.get(0, 1), 0);

	b.add(1, 1);
	EXPECT_EQ(b.count(), 1);
	EXPECT_EQ(b.size(), 2);
	EXPECT_EQ(b.get(0, 1), 0);
	EXPECT_EQ(b.get(1, 1), 1);
	EXPECT_EQ(b.get(0, 2), 2);

	b.set_bit(0, true);
	EXPECT_EQ(b.count(), 2);
	EXPECT_EQ(b.size(), 2);
	EXPECT_EQ(b.get(0, 1), 1);
	EXPECT_EQ(b.get(1, 1), 1);
	EXPECT_EQ(b.get(0, 2), 3);
}

TEST(bit_vector, inter_byte) {
	bit_vector b;
	b.add(0xff, 8);
	b.add(0, 1);
	EXPECT_EQ(b.count(), 8);
	EXPECT_EQ(b.size(), 9);
	for (int i = 0; i < 8; ++i) {
		EXPECT_EQ(b.get(i, 1), 1);
	}
	EXPECT_EQ(b.get(8, 1), 0);
	EXPECT_EQ(b.get(7, 2), 1);
	b.add(1, 1);
	EXPECT_EQ(b.get(7, 3), 5);
}

TEST(bit_vector, misc) {
	bit_vector b;
	b.add(0b10110111010010101, 17);
	EXPECT_EQ(b.count(), 10);
	EXPECT_EQ(b.size(), 17);
	EXPECT_EQ(b.get(0, 8), 0b10010101);
	EXPECT_EQ(b.get(8, 8), 0b01101110);
	EXPECT_EQ(b.get(16, 1), 0b1);
	EXPECT_EQ(b.get(0, 17), 0b10110111010010101);
	EXPECT_EQ(b.get(0, 16), 0b110111010010101);
	EXPECT_EQ(b.get(0, 15), 0b110111010010101);
	EXPECT_EQ(b.get(0, 14), 0b10111010010101);
	EXPECT_EQ(b.get(1, 14), 0b11011101001010);
}


}
