#include <statistics/andersondarling.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace mixer {

TEST(anderson_darling, basic) {
	EXPECT_NEAR(anderson_darling_stats({.1,.2,.3,.4,.5,.6,.7,.8,.9}), 0.1549, 1e-4);
}

TEST(anderson_darling, no_change) {
	const auto r = anderson_darling_test(50, test_stream());
	EXPECT_NEAR(r->value,  0.5850, 1e-4);
	EXPECT_NEAR(r->p_value, 0.3385, 1e-4);
}


}
