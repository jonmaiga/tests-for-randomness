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

TEST(regularized_beta, basic) {
	EXPECT_NEAR(beta_regularized(1, 1, 1), 1., 1e-4);
	EXPECT_NEAR(beta_regularized(0.01, 0.01, 0.5), .5, 1e-4);
	EXPECT_NEAR(beta_regularized(0.01, 0.02, 0.5), .6667, 1e-4);
	EXPECT_NEAR(beta_regularized(1, 3, 0.4), 0.784, 1e-4);
	EXPECT_NEAR(beta_regularized(10, 12, 0.5), .6681, 1e-4);
	EXPECT_NEAR(beta_regularized(10, 12, 0.99), 1, 1e-4);
	EXPECT_NEAR(beta_regularized(12, 10, 0.99), 1., 1e-4);
	EXPECT_NEAR(beta_regularized(900, 1024, 0.23), 0, 1e-4);
}

TEST(gamma_regularized, basic) {
	EXPECT_NEAR(1.-gamma_regularized(1, 1), 0.3678, 1e-4);
	EXPECT_NEAR(1.-gamma_regularized(4, 0.88), 0.9875, 1e-4);
	EXPECT_NEAR(1.-gamma_regularized(40, 3.88), 1, 1e-4);
	EXPECT_NEAR(1.-gamma_regularized(0.5, 2.111), 0.03995, 1e-4);
}

TEST(binomial_pdf, basic) {
	EXPECT_NEAR(binomial_pdf(40, 0.5, 20), 0.1253, 1e-4);
	EXPECT_NEAR(binomial_pdf(1, 0, 0), 1, 1e-4);
	EXPECT_NEAR(binomial_pdf(1, 1, 0), 0, 1e-4);
}


TEST(students_t_cdf, basic) {
	EXPECT_NEAR(student_t_cdf(1, 5), 0.3632, 1e-4);
	EXPECT_NEAR(student_t_cdf(1.1, 120), 0.2735, 1e-4);
	EXPECT_NEAR(student_t_cdf(0.615227, 4), 0.571683, 1e-4); // matches mma TTest (given t-score)
}

TEST(f_distribution_cdf, basic) {
	EXPECT_NEAR(f_distribution_cdf(0.035082, 4, 4), 0.0033683, 1e-4); // matches mma FTest (given f-score)
}

TEST(chi2_distribution_cdf, basic) {
	EXPECT_NEAR(chi2_distribution_cdf(0.1, 5), 1-0.0001623, 1e-4); // matches mma CDF(chi2)
	EXPECT_NEAR(chi2_distribution_cdf(10.1, 5), 1-0.9275, 1e-4); // matches mma CDF(chi2)
	EXPECT_NEAR(chi2_distribution_cdf(13.1, 3.3), 1-0.994, 1e-4); // matches mma CDF(chi2)
	EXPECT_NEAR(chi2_distribution_cdf(3.71429, 4), 0.446052, 1e-4); // matches mma PearsonChiSquareTest given chi2 statistic (p=0.446052)
}

TEST(kolmogorov_smirnov_cdf, basic) {
	// there are links to ks cdf implementations here: https://en.wikipedia.org/wiki/Kolmogorov%E2%80%93Smirnov_test#Kolmogorov_distribution 
	EXPECT_NEAR(kolmogorov_smirnov_cdf(0.4, 5, 1000), 0.3128, 1e-4); // boost have a theta function maybe use that (doesn't match mma right now 0.8095)
}


}
