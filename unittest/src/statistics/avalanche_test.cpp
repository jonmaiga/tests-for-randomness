#include <statistics/avalanche.h>

#include <gtest/gtest.h>

#include "mixers.h"
#include "testutil.h"

namespace mixer {

TEST(avalanche, sac_data_no_change) {
	EXPECT_EQ(hash64(avalanche_generate_sac(10, test_stream(), mx3)), 15512756382040012752ull);
}

TEST(avalanche, bic_data_no_change) {
	EXPECT_EQ(hash64(avalanche_generate_bic(10, test_stream(), mx3)), 13882906137123132453ull);
}

TEST(avalanche, sac_no_change) {
	const auto r = avalanche_mixer_sac_test(50, test_stream(), mx3);
	EXPECT_EQ(r.size(), 1);
	EXPECT_NEAR(r.front().value,  22.4233, 1e-4);
	EXPECT_NEAR(r.front().p_value, 0.4348, 1e-4);
}

TEST(avalanche, bic_no_change) {
	const auto r = avalanche_mixer_bic_test(50, test_stream(), mx3);
	EXPECT_EQ(r.size(), 1);
	EXPECT_NEAR(r.front().value,  3939.200, 1e-4);
	EXPECT_NEAR(r.front().p_value, 0.9588, 1e-4);
}


}
