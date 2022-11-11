#include <util/math.h>

#include <gtest/gtest.h>

namespace mixer {

TEST(math, rescale_basic) {
	EXPECT_EQ(rescale_to_01(0, 0, 0, 0), 0.5);
	EXPECT_EQ(rescale_to_01(1, 2, 2, 0), 0.5);
	EXPECT_EQ(rescale_to_01(-1, -4, -4, 0), 0.5);

	EXPECT_EQ(rescale_to_01(0, 0, 1, 0), 0);
	EXPECT_EQ(rescale_to_01(.5, 0, 1, 0), 0.5);
	EXPECT_EQ(rescale_to_01(1, 0, 1, 0), 1);
	EXPECT_EQ(rescale_to_01(33, 0, 66, 0), .5);
	EXPECT_EQ(rescale_to_01(33, 3, 63, 0), .5);
}

TEST(math, rescale_epsilon) {
	EXPECT_EQ(rescale_to_01(0, 0, 0, 0.1), 0.5);
	EXPECT_EQ(rescale_to_01(0, 0, 1, 0.1), 0.1);
	EXPECT_EQ(rescale_to_01(1, 0, 1, 0.1), 0.9);
	EXPECT_NEAR(rescale_to_01(5, 1, 6, 0.1), 0.74, 1e-4);
	EXPECT_NEAR(rescale_to_01(6, 1, 6, 0.1), 0.9, 1e-4);
	EXPECT_NEAR(rescale_to_01(2, 1, 6, 0.1), 0.26, 1e-4);
}

}
