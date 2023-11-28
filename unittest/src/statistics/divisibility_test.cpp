#include <statistics/divisibility.h>

#include <gtest/gtest.h>

#include "testutil.h"


namespace tfr {
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
	auto r = divisibility_test(10000, test_stream()).front().stats;
	EXPECT_NEAR(r->value, 26.0893, 1e-4);
	EXPECT_NEAR(r->p_value, 0.03709, 1e-4);

	r = divisibility_test(10000, test_stream()).back().stats;
	EXPECT_NEAR(r->value, 18.6600, 1e-4);
	EXPECT_NEAR(r->p_value, 0.6662, 1e-4);
}

TEST(divisibility, divisibility_no_change_8) {
	auto s = test_stream();
	std::vector<uint8_t> data;
	for (int i = 0; i < (1<<22); ++i) {
		data.push_back(static_cast<uint8_t>(s()));
	}
	const auto s8 = create_stream_from_data_by_ref("test", data);
	auto r = divisibility_test<uint8_t>(data.size(), s8).front().stats;
	EXPECT_NEAR(r->value, 11.1800, 1e-4);
	EXPECT_NEAR(r->p_value, 0.7397, 1e-4);
}
}
