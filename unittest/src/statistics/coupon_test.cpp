#include <statistics/coupon.h>

#include <gtest/gtest.h>

#include "testutil.h"


namespace mixer {

TEST(coupon, collect_coupons) {
	using D = std::vector<double>;
	using T = std::vector<uint64_t>;
	EXPECT_EQ(collect_coupons<D>(0, 0, D{}), (T{ }));
	EXPECT_EQ(collect_coupons<D>(0, 1, { 0.5 }), (T{0}));

	EXPECT_EQ(collect_coupons<D>(1, 1, D{}), (T{ 0 }));
	EXPECT_EQ(collect_coupons<D>(1, 1, { .5 }), (T{ 1 }));

	EXPECT_EQ(collect_coupons<D>(2, 2, {0.3, 0.4, 0.7}), (T{0, 1}));
	EXPECT_EQ(collect_coupons<D>(2, 2, {0.3, 0.7, 0.4, 0.7}), (T{2, 0}));
}

TEST(coupon, expected_probabilities) {
	const auto ps = expected_probabilities(5);
	EXPECT_EQ(ps.size(), 25);
	EXPECT_NEAR(std::accumulate(ps.begin(), ps.end(), 0., std::plus()), 1, 1e-4);

	// matches mma
	EXPECT_NEAR(ps[0], 0.0384, 1e-4);
	EXPECT_NEAR(ps[1], 0.0768, 1e-4);
	EXPECT_NEAR(ps[15], 0.0141, 1e-4);
}

TEST(coupon, expected_draws) {
	EXPECT_NEAR(expected_draws_per_coupon(1), 1.0772, 1e-4);
	EXPECT_NEAR(expected_draws_per_coupon(5), 11.4332, 1e-4);
	EXPECT_NEAR(expected_draws_per_coupon(40), 171.1438, 1e-4);
	EXPECT_NEAR(expected_draws_per_coupon(60), 280.7936, 1e-4);
}

TEST(coupon, expected_probabilities_10_20) {
	const auto ps = expected_probabilities(10);
	EXPECT_EQ(ps.size(), 21);
	EXPECT_NEAR(std::accumulate(ps.begin(), ps.end(), 0., std::plus()), 1, 1e-4);

	// matches mma
	EXPECT_NEAR(ps[0], 0.000362, 1e-4);
	EXPECT_NEAR(ps[1], 0.001632, 1e-4);
	EXPECT_NEAR(ps[15], 0.04377, 1e-4);
}

TEST(coupon, coupon_no_change) {
	const auto r = coupon_test(10000, test_stream()).front().stats;
	EXPECT_NEAR(r->value, 32.5748, 1e-4);
	EXPECT_NEAR(r->p_value, 0.1133, 1e-4);
}

}
