#include <statistics/basic.h>

#include <gtest/gtest.h>

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

}
