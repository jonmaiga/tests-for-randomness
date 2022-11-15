#include <statistics/divisibility.h>

#include <gtest/gtest.h>

#include "testutil.h"


namespace mixer {

TEST(divisibility, collect_divisbility) {
	using T = std::vector<uint64_t>;
	EXPECT_EQ(collect_divisible(1, 4, {1,3,5,2,4}), T({1,0,0,1,0}));
	EXPECT_EQ(collect_divisible(1, 4, {1,3,5,2,4,6}), T({2,0,0,1,0}));
	EXPECT_EQ(collect_divisible(1, 4, {1,3,5,1,1,4}), T({0,0,0,0,1}));
}

TEST(divisibility, expected_probabilities) {
	const auto ps = divisible_expected_probabilities(10, 20);
	EXPECT_EQ(ps.size(), 21);
	EXPECT_NEAR(std::accumulate(ps.begin(), ps.end(), 0., std::plus()), 1, 1e-4);
}

TEST(divisibility, divisibility_no_change) {
	const auto r = divisibility_test(10000, test_stream());
	EXPECT_EQ(r.size(), 1);
	//EXPECT_NEAR(r.front().value, 27.4632, 1e-4);
	//EXPECT_NEAR(r.front().p_value, 0.1227, 1e-4);

}

}
