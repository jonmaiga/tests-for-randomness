#include <util/math.h>

#include <gtest/gtest.h>

namespace mixer {

TEST(math, normalize_basic) {
	EXPECT_EQ(normalize(0, 0, 0, 0), 0.5);
	EXPECT_EQ(normalize(1, 2, 2, 0), 0.5);
	EXPECT_EQ(normalize(-1, -4, -4, 0), 0.5);

	EXPECT_EQ(normalize(0, 0, 1, 0), 0);
	EXPECT_EQ(normalize(.5, 0, 1, 0), 0.5);
	EXPECT_EQ(normalize(1, 0, 1, 0), 1);
	EXPECT_EQ(normalize(33, 0, 66, 0), .5);
	EXPECT_EQ(normalize(33, 3, 63, 0), .5);
}

TEST(math, normalize_epsilon) {
	EXPECT_EQ(normalize(0, 0, 0, 0.1), 0.5);
	EXPECT_EQ(normalize(0, 0, 1, 0.1), 0.1);
	EXPECT_EQ(normalize(1, 0, 1, 0.1), 0.9);
	EXPECT_NEAR(normalize(5, 1, 6, 0.1), 0.74, 1e-4);
	EXPECT_NEAR(normalize(6, 1, 6, 0.1), 0.9, 1e-4);
	EXPECT_NEAR(normalize(2, 1, 6, 0.1), 0.26, 1e-4);
}

}
