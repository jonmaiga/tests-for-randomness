#include <statistics/avalanche.h>

#include <gtest/gtest.h>

#include "mixers64.h"
#include "testutil.h"

namespace mixer {

TEST(avalanche, sac_data_no_change) {
	EXPECT_EQ(hash64(avalanche_generate_sac(10, test_stream(), mx3)), 1240375597136458279ull);
}

TEST(avalanche, bic_data_no_change) {
	EXPECT_EQ(hash64(avalanche_generate_bic(10, test_stream(), mx3)), 3526828816776552911ull);
}

TEST(avalanche, sac_data_large) {
	using T = uint32_t;
	constexpr auto n = 200000;
	const auto counts = avalanche_generate_sac<T>(n, test_stream<T>(), mix32::mx3);
	auto s = avalanche_sac_stats<T>(n, counts);
	EXPECT_NEAR(s->value, 17.7571, 1e-4);
	EXPECT_NEAR(s->p_value, 0.8844, 1e-4);
}

TEST(avalanche, sac_no_change) {
	const auto r = avalanche_mixer_sac_test(50, test_stream(), mx3).front().stats;
	EXPECT_NEAR(r->value,  17.1819, 1e-4);
	EXPECT_NEAR(r->p_value, 0.7532, 1e-4);
}

TEST(avalanche, bic_no_change) {
	const auto r = avalanche_mixer_bic_test(50, test_stream(), mx3).front().stats;
	EXPECT_NEAR(r->value,  4020.4800, 1e-4);
	EXPECT_NEAR(r->p_value, 0.7954, 1e-4);
}

}
