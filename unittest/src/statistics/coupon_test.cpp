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
	EXPECT_NEAR(expected_draws_per_coupon(1), 1, 1e-4);
	EXPECT_NEAR(expected_draws_per_coupon(5), 11.4166, 1e-4); // mma 11.41666...
	EXPECT_NEAR(expected_draws_per_coupon(10), 29.289682539682539683, 1e-4);
	EXPECT_NEAR(expected_draws_per_coupon(40), 171.1417, 1e-4); // mma 171.1417
	EXPECT_NEAR(expected_draws_per_coupon(60), 280.7922, 1e-4); // mma 280.7922
}

TEST(coupon, expected_draws_large) {
	EXPECT_NEAR(expected_draws_per_coupon(1ull << 10), 7689.3959, 1e-4); // mma 7689.3958
	EXPECT_NEAR(expected_draws_per_coupon(1ull << 20), 15141604.9530, 1e-4); // mma 15141604.9530
	EXPECT_NEAR(expected_draws_per_coupon(1ull << 25), 600822142.6795, 1e-4); // mma 6.008221426795335e8
	EXPECT_NEAR(expected_draws_per_coupon(1ull << 29), 11101676511.28232, 1e-4); // mma 1.110167651128232e10
	EXPECT_NEAR(expected_draws_per_coupon(1ull << 40), 31119590726726.6085, 1e-4); // mma 3.11195907267266085584380946871e13
	EXPECT_NEAR(expected_draws_per_coupon(1ull << 50), 39670604364374234.6749, 1e-4); // mma 3.96706043643742346749420681270e16 Last integer digit is not correct
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
	EXPECT_NEAR(r->value, 29.3991, 1e-4);
	EXPECT_NEAR(r->p_value, 0.2054, 1e-4);
}

}
