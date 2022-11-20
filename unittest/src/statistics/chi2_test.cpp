#include <statistics/chi2.h>

#include <gtest/gtest.h>

#include "testutil.h"

namespace mixer {

TEST(chi2, unset) {
	EXPECT_FALSE(chi2_stats({}, 0));
	EXPECT_FALSE(chi2_stats({1}, 0));
}

TEST(chi2, basic) {
	using T = std::vector<double>;
	// same as mma
	EXPECT_EQ(chi2_uniform_stats<T>({0.1})->value, 1);
	EXPECT_EQ(chi2_uniform_stats<T>({0.1})->df, 1);
	EXPECT_EQ(chi2_uniform_stats<T>({0.1, 0.1, 0.1, 0.1})->value, 12);
	EXPECT_EQ(chi2_uniform_stats<T>({0.1, 0.1, 0.1, 0.1})->df, 3);
	EXPECT_NEAR(chi2_uniform_stats<T>({0.1, 0.2, 0.3, 0.4, 0.5, 0.4, 0.8})->value, 3.71428, 1e-4);
	EXPECT_EQ(chi2_uniform_stats<T>({0.1, 0.2, 0.3, 0.4, 0.5, 0.4, 0.8})->df, 4);
}

TEST(chi2, no_change) {
	const auto r = chi2_test(50, test_stream());
	EXPECT_NEAR(r->value, 4.7999, 1e-4);
	EXPECT_NEAR(r->p_value, 0.8513, 1e-4);
}


}
