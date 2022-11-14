#include <statistics/coupon.h>

#include <gtest/gtest.h>

#include "testutil.h"


namespace mixer {

TEST(coupon, collect_coupons) {
	using T = std::vector<uint64_t>;
	EXPECT_EQ(collect_coupons(0, 0, {}), (T{ 0 }));
	EXPECT_EQ(collect_coupons(0, 1, { 0.5 }), (T{ 0, 0 }));

	EXPECT_EQ(collect_coupons(1, 1, {}), (T{ 0, 0 }));
	EXPECT_EQ(collect_coupons(1, 1, { .5 }), (T{ 1, 0 }));

	EXPECT_EQ(collect_coupons(2, 2, {0.3, 0.4, 0.7}), (T{0, 1, 0}));
	EXPECT_EQ(collect_coupons(2, 2, {0.3, 0.7, 0.4, 0.7}), (T{2, 0, 0}));
}

TEST(coupon, expected_probabilities) {
	const auto ps = expected_probabilities(5, 20);
	EXPECT_EQ(ps.size(), 21);
	EXPECT_NEAR(std::accumulate(ps.begin(), ps.end(), 0., std::plus()), 1, 1e-4);

	// matches mma
	EXPECT_NEAR(ps[0], 0.0384, 1e-4);
	EXPECT_NEAR(ps[1], 0.0768, 1e-4);
	EXPECT_NEAR(ps[15], 0.0141, 1e-4);
}

TEST(coupon, coupon_no_change) {
	const auto r = coupon_test(10000, test_stream());
	EXPECT_EQ(r.size(), 1);
	EXPECT_NEAR(r.front().value, 27.4632, 1e-4);
	EXPECT_NEAR(r.front().p_value, 0.1227, 1e-4);
}

}
