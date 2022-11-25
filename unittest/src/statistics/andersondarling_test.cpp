#include <statistics/andersondarling.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace mixer {

TEST(anderson_darling, unset) {
	EXPECT_FALSE(anderson_darling_stats({}));
	// EXPECT_FALSE(anderson_darling_stats({1.}));
}

TEST(anderson_darling, basic) {
	EXPECT_NEAR(anderson_darling_stats({.1, .2})->value, 1.8350, 1e-4);
	EXPECT_NEAR(anderson_darling_stats({.1,.2,.3,.4,.5,.6,.7,.8,.9})->value, 0.1549, 1e-4);
}

TEST(anderson_darling, large) {
	auto s = test_stream();
	std::vector<double> a;
	while (a.size() < 1000000) {
		a.push_back(rescale_type_to_01(s()));
	}
	const auto stat = anderson_darling_stats(a);
	EXPECT_NEAR(stat->value, 0.6502, 1e-4);
	EXPECT_NEAR(stat->p_value, 0.3982, 1e-4);
}

TEST(anderson_darling, no_change) {
	const auto r = anderson_darling_test(50, test_stream()).front().stats;
	EXPECT_NEAR(r->value, 0.5850, 1e-4);
	EXPECT_NEAR(r->p_value, 0.3385, 1e-4);
}


}
