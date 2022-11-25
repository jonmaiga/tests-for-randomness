#include <statistics/kolmogorov.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace mixer {

TEST(kolmogorov, large) {
	auto s = test_stream();
	std::vector<double> a;
	while (a.size() < 1000000) {
		a.push_back(rescale_type_to_01(s()));
	}
	const auto stat = kolmogorov_smirnov_stats(a);
	EXPECT_NEAR(stat->value, 0.0007142, 1e-4);
	EXPECT_NEAR(stat->p_value, 0.6873, 1e-4);
}

TEST(kolmogorov, no_change) {
	const auto r = kolmogorov_test(50, test_stream()).front().stats;
	EXPECT_NEAR(r->value, 0.0971, 1e-4);
	EXPECT_NEAR(r->p_value, 0.7113, 1e-4);
}

}
