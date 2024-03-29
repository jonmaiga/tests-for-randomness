#include <statistics/basic.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace tfr {

TEST(statistics_basic, median) {
	using T = std::vector<double>;
	EXPECT_EQ(get_median<T>({1}), 1);
	EXPECT_EQ(get_median<T>({1,1}), 1);
	EXPECT_EQ(get_median<T>({1,1,1}), 1);
	EXPECT_EQ(get_median<T>({1,2}), 1.5);
	EXPECT_EQ(get_median<T>({1,2,3}), 2);
	EXPECT_EQ(get_median<T>({1,2,3,4}), 2.5);
	EXPECT_EQ(get_median<T>({1,2,4,5}), 3);
	EXPECT_EQ(get_median<T>({1, 1, 1, 2, 2 ,2}), 1.5);
}

TEST(statistics_basic, sum_of_squares) {
	using T = std::vector<double>;
	EXPECT_EQ(get_sum_of_squares<T>({1, 2}, 1.5), .5);
	EXPECT_EQ(get_sum_of_squares<T>({1, 2, 3}, 2), 2);
	EXPECT_NEAR(get_sum_of_squares<T>({1, 2, 3, 4}, 2.5), 5, 1e-4);
	EXPECT_NEAR(get_sum_of_squares<T>({-3,-1,-1,3}, -0.5), 19, 1e-4);
}

TEST(statistics_basic, no_change) {
	const auto r = mean_test(50, test_stream()).front().stats;
	EXPECT_NEAR(r->value, 0.03325, 1e-4);
	EXPECT_NEAR(r->p_value, 0.9734, 1e-4);
}

}
