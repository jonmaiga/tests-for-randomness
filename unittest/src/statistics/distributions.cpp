#include <statistics/distributions.h>

#include <gtest/gtest.h>

namespace mixer {

TEST(normal_cdf, basic) {
	EXPECT_NEAR(normal_cdf(-10), 0., 1e-4);
	EXPECT_NEAR(normal_cdf(-1.644), 0.05, 1e-4);
	EXPECT_NEAR(normal_cdf(-1), 0.1586, 1e-4);
	EXPECT_NEAR(normal_cdf(0), .5000, 1e-4);
	EXPECT_NEAR(normal_cdf(1), 0.8413, 1e-4);
	EXPECT_NEAR(normal_cdf(1.644), 0.95, 1e-4);
	EXPECT_NEAR(normal_cdf(10), 1., 1e-4);
}

TEST(two_tailed_normal_cdf, basic) {
	EXPECT_NEAR(two_tailed_normal_cdf(-10), 0., 1e-4);
	EXPECT_NEAR(two_tailed_normal_cdf(-1.64485), 0.1, 1e-4);
	EXPECT_NEAR(two_tailed_normal_cdf(-1), 0.3173, 1e-4);
	EXPECT_NEAR(two_tailed_normal_cdf(0), 1, 1e-4);
	EXPECT_NEAR(two_tailed_normal_cdf(1), 0.3173, 1e-4);
	EXPECT_NEAR(two_tailed_normal_cdf(1.64485), 0.1, 1e-4);
	EXPECT_NEAR(two_tailed_normal_cdf(10), 0., 1e-4);
}

TEST(incomplete_beta, basic) {
	EXPECT_NEAR(incomplete_beta(1, 1, 1), 1., 1e-4);
	EXPECT_NEAR(incomplete_beta(0.01, 0.01, 0.5), .5, 1e-4);
	EXPECT_NEAR(incomplete_beta(0.01, 0.02, 0.5), .6667, 1e-4);
	EXPECT_NEAR(incomplete_beta(1, 3, 0.4), 0.784, 1e-4);
	EXPECT_NEAR(incomplete_beta(10, 12, 0.5), .6681, 1e-4);
	EXPECT_NEAR(incomplete_beta(10, 12, 0.99), 1, 1e-4);
	EXPECT_NEAR(incomplete_beta(12, 10, 0.99), 1., 1e-4);
	EXPECT_NEAR(incomplete_beta(900, 1024, 0.23), 0, 1e-4);
}

}
