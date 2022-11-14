#include <util/math.h>

#include <gtest/gtest.h>

namespace mixer {

TEST(math, rescale_basic) {
	EXPECT_EQ(rescale_to_01(0, 0, 0), 0.5);
	EXPECT_EQ(rescale_to_01(1, 2, 2), 0.5);
	EXPECT_EQ(rescale_to_01(-1, -4, -4), 0.5);

	EXPECT_EQ(rescale_to_01(0, 0, 1), 0);
	EXPECT_EQ(rescale_to_01(.5, 0, 1), 0.5);
	EXPECT_EQ(rescale_to_01(1, 0, 1), 1);
	EXPECT_EQ(rescale_to_01(33, 0, 66), .5);
	EXPECT_EQ(rescale_to_01(33, 3, 63), .5);
}

TEST(math, snap_to_01) {
	EXPECT_EQ(snap_to_01(0.5), 0.5);
	EXPECT_EQ(snap_to_01(0.1), 0.1);
	EXPECT_EQ(snap_to_01(0.9), 0.9);
	EXPECT_EQ(snap_to_01(0), 0);
	EXPECT_EQ(snap_to_01(1), 1);
	EXPECT_EQ(snap_to_01(1 - 1e-7), 1 - 1e-7);
	EXPECT_EQ(snap_to_01(1 + 1e-7), 1);
	EXPECT_EQ(snap_to_01(0 - 1e-7), 0);
	EXPECT_EQ(snap_to_01(0 + 1e-7), 0 + 1e-7);
}

TEST(math, stirling_second_kind) {
	EXPECT_EQ(stirling_second_kind(0, 0), 1);
	EXPECT_EQ(stirling_second_kind(0, 1), 0);
	EXPECT_EQ(stirling_second_kind(1, 0), 0);
	EXPECT_EQ(stirling_second_kind(1, 1), 1);
	EXPECT_EQ(stirling_second_kind(4, 2), 7);
	EXPECT_EQ(stirling_second_kind(4, 3), 6);
	EXPECT_EQ(stirling_second_kind(20, 9), 12011282644725);
	EXPECT_EQ(stirling_second_kind(20, 10), 5917584964655);
	EXPECT_EQ(stirling_second_kind(20, 11), 1900842429486);
}

}
