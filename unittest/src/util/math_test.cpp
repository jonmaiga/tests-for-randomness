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

}
