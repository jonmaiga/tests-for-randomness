#include <statistics/correlation.h>

#include <gtest/gtest.h>

namespace mixer {

TEST(pearson_correlation, basic) {
	EXPECT_NEAR(pearson_correlation({1,2,3}, {1,2,3}), 1, 1e-4);
	EXPECT_NEAR(pearson_correlation({3,2,1}, {3,2,1}), 1, 1e-4);
	EXPECT_NEAR(pearson_correlation({1,2,3}, {3,2,1}), -1, 1e-4);
	EXPECT_NEAR(pearson_correlation({1,2,3}, {1,2,4}), 0.9819, 1e-4);
	EXPECT_NEAR(pearson_correlation({1,2,3,5}, {1,2,4,5}), 0.9621, 1e-4);
	EXPECT_NEAR(pearson_correlation({1,2,3,5}, {5,4,2,1}), -0.9621, 1e-4);
}

TEST(pearson_correlation, p_value) {
	// same as mma
	EXPECT_NEAR(correlation_p_value(0.9861, 6), 0.0002862, 1e-4);
	EXPECT_NEAR(correlation_p_value(0.5727, 6), 0.2348, 1e-4);
	EXPECT_NEAR(correlation_p_value(-0.9621, 4), 0.03785, 1e-4);
	EXPECT_NEAR(correlation_p_value(0.9621, 4), 0.03785, 1e-4);
}

}
