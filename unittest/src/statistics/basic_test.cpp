#include <statistics/basic.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace mixer {

TEST(statistics_basic, median) {
	EXPECT_EQ(get_median<double>({1}), 1);
	EXPECT_EQ(get_median<double>({1,1}), 1);
	EXPECT_EQ(get_median<double>({1,1,1}), 1);
	EXPECT_EQ(get_median<double>({1,2}), 1.5);
	EXPECT_EQ(get_median<double>({1,2,3}), 2);
	EXPECT_EQ(get_median<double>({1,2,3,4}), 2.5);
	EXPECT_EQ(get_median<double>({1,2,4,5}), 3);
	EXPECT_EQ(get_median<double>({1, 1, 1, 2, 2 ,2}), 1.5);
}

TEST(statistics_basic, sum_of_squares) {
	EXPECT_EQ(get_sum_of_squares<double>({1, 2}, 1.5), .5);
	EXPECT_EQ(get_sum_of_squares<double>({1, 2, 3}, 2), 2);
	EXPECT_NEAR(get_sum_of_squares<double>({1, 2, 3, 4}, 2.5), 5, 1e-4);
	EXPECT_NEAR(get_sum_of_squares<double>({-3,-1,-1,3}, -0.5), 19, 1e-4);
}

TEST(statistics_basic, no_change) {
	const auto r = basic_test(50, test_stream());
	EXPECT_EQ(r.size(), 1);
	EXPECT_NEAR(r.front().value,  0.5013, 1e-4);
	EXPECT_NEAR(*r.front().p_value, 0.9734, 1e-4);
}


}
