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

}
