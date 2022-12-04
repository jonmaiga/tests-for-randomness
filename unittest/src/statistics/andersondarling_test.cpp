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
	EXPECT_NEAR(stat->p_value, 1-0.3982, 1e-4);
}

TEST(anderson_darling, data) {
	const auto pv = {
		0.0907329,0.932244,0.114827,0.568087,0.776664,0.952185,0.104311,0.039066,0.0191372,0.6889,0.469074,0.167117,0.800801,0.676952,0.731704,0.354055,0.928046,0.285814,0.858386,0.908818,0.07048,0.481376,0.728044,0.646651,0.707484,0.385696,0.428934,0.56704,0.757637,0.7868,0.011541,0.298083,0.565069,0.977096,0.751907,0.149083,0.000146674,0.1505,0.423413,0.786884,0.064658,0.624864,0.727396,0.244652,0.491885,0.189494,0.365173,0.596297,0.937726,0.53262,0.361511,0.24349,0.654869,0.830458,0.738669,0.78607,0.549001,0.563609,0.034134,0.643928,0.748101,0.866515,0.255059,0.213638,0.399358,0.127307,0.0803425,0.535384,0.479835,0.332602,0.313224,0.74566,0.586695,0.31661,0.622685,0.319371,0.387137,0.956242,0.852309,0.117861,0.34173,0.278867,0.590599,0.846015,0.308734,0.346514,0.583689,0.632231,0.257446,0.499904,0.0834331,0.262653,0.1925,0.332932,0.786185,0.584509,0.288786,0.538318,0.292864,0.102028
	};
	const auto s = anderson_darling_stats(pv);
	EXPECT_NEAR(s->value, 0.719128, 1e-4); // matches mma
	EXPECT_NEAR(s->p_value, 0.54268, 1e-4); // not quite mma (0.542522)
}


}
