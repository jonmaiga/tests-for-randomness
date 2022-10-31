#include <statistics/chi2.h>

#include <gtest/gtest.h>

namespace mixer {

TEST(chi2, basic) {
	// same as mma
	EXPECT_EQ(compute_chi2({0.1}).chi2, 1);
	EXPECT_EQ(compute_chi2({0.1}).df, 1);
	EXPECT_EQ(compute_chi2({0.1, 0.1, 0.1, 0.1}).chi2, 12);
	EXPECT_EQ(compute_chi2({0.1, 0.1, 0.1, 0.1}).df, 3);
	EXPECT_NEAR(compute_chi2({0.1, 0.2, 0.3, 0.4, 0.5, 0.4, 0.8}).chi2, 3.71428, 1e-4);
	EXPECT_EQ(compute_chi2({0.1, 0.2, 0.3, 0.4, 0.5, 0.4, 0.8}).df, 4);
}

}
