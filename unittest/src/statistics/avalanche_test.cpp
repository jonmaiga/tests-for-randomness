#include <statistics/avalanche.h>

#include <gtest/gtest.h>

#include "mixers.h"
#include "testutil.h"

namespace mixer {

TEST(avalanche, sac_data_no_change) {
	EXPECT_EQ(hash64(avalanche_generate_sac(10, test_stream(), mx3)), 1240375597136458279ull);
}

TEST(avalanche, bic_data_no_change) {
	EXPECT_EQ(hash64(avalanche_generate_bic(10, test_stream(), mx3)), 3526828816776552911ull);
}

TEST(avalanche, sac_no_change) {
	const auto r = avalanche_mixer_sac_test(50, test_stream(), mx3);
	EXPECT_NEAR(r->value,  17.1819, 1e-4);
	EXPECT_NEAR(r->p_value, 0.7532, 1e-4);
}

TEST(avalanche, bic_no_change) {
	const auto r = avalanche_mixer_bic_test(50, test_stream(), mx3);
	EXPECT_NEAR(r->value,  4020.4800, 1e-4);
	EXPECT_NEAR(r->p_value, 0.7954, 1e-4);
}

}
