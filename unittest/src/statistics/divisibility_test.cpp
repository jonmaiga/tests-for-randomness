#include <statistics/divisibility.h>

#include <gtest/gtest.h>

#include "testutil.h"


namespace mixer {

TEST(divisibility, collect_divisbility) {
	using T = std::vector<uint64_t>;
	EXPECT_EQ(collect_divisible<T>(2, 1, 4, {1,3,5,2,4}), T({1,0,0,1}));
	EXPECT_EQ(collect_divisible<T>(2, 1, 4, {1,3,5,2,4,6}), T({2,0,0,1}));
	EXPECT_EQ(collect_divisible<T>(2, 1, 4, {1,3,5,1,1,4}), T({0,0,0,1}));
}

TEST(divisibility, expected_probabilities) {
	const auto ps = divisible_expected_probabilities(2, 10);
	EXPECT_EQ(ps.size(), 25);
	EXPECT_NEAR(std::accumulate(ps.begin(), ps.end(), 0., std::plus()), 1, 1e-4);
}

TEST(divisibility, divisibility_no_change) {
	auto r = divisibility_test(1000, test_stream(), 2);
	EXPECT_NEAR(r->value, 8.8334, 1e-4);
	EXPECT_NEAR(r->p_value, 0.7854, 1e-4);

	r = divisibility_test(1000, test_stream(), 3);
	EXPECT_NEAR(r->value, 10.1544, 1e-4);
	EXPECT_NEAR(r->p_value, 0.8970, 1e-4);
}

}
