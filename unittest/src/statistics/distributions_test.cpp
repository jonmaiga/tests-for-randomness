#include <statistics/distributions.h>

#include <gtest/gtest.h>

namespace mixer {

TEST(normal_cdf, basic) {
	// matches mma CDF[NormalDistribution[]], x]
	EXPECT_NEAR(normal_cdf(-10), 0., 1e-4);
	EXPECT_NEAR(normal_cdf(-1.644), 0.0500, 1e-4);
	EXPECT_NEAR(normal_cdf(-1), 0.1586, 1e-4);
	EXPECT_NEAR(normal_cdf(0), .5000, 1e-4);
	EXPECT_NEAR(normal_cdf(1), 0.8413, 1e-4);
	EXPECT_NEAR(normal_cdf(1.644), 0.9500, 1e-4);
	EXPECT_NEAR(normal_cdf(10), 1., 1e-4);
}

TEST(normal_two_tailed_cdf, basic) {
	EXPECT_NEAR(normal_two_tailed_cdf(-10), 0., 1e-4);
	EXPECT_NEAR(normal_two_tailed_cdf(-1.64485), 0.1, 1e-4);
	EXPECT_NEAR(normal_two_tailed_cdf(-1), 0.3173, 1e-4);
	EXPECT_NEAR(normal_two_tailed_cdf(0), 1, 1e-4);
	EXPECT_NEAR(normal_two_tailed_cdf(1), 0.3173, 1e-4);
	EXPECT_NEAR(normal_two_tailed_cdf(1.64485), 0.1, 1e-4);
	EXPECT_NEAR(normal_two_tailed_cdf(10), 0., 1e-4);
}

TEST(regularized_beta, basic) {
	// matches mma BetaRegularized[...]
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
	// matches mma GammaRegularized[...]
	EXPECT_NEAR(gamma_regularized(1, 1), 0.3678, 1e-4);
	EXPECT_NEAR(gamma_regularized(4, 0.88), 0.9875, 1e-4);
	EXPECT_NEAR(gamma_regularized(40, 3.88), 1, 1e-4);
	EXPECT_NEAR(gamma_regularized(0.5, 2.111), 0.03995, 1e-4);
	EXPECT_NEAR(gamma_regularized(0, 1), 0, 1e-4);
	EXPECT_NEAR(gamma_regularized(1, 0), 1, 1e-4);
	EXPECT_NEAR(gamma_regularized(0, 0), 0, 1e-4);
}

TEST(beta, basic) {
	// matches mma Beta[...] (wolframalpha)
	EXPECT_NEAR(beta(1, 1), 1, 1e-4);
	EXPECT_NEAR(beta(0.5, 1), 2, 1e-4);
	EXPECT_NEAR(beta(0.25, 0.25), 7.4163, 1e-4);
	EXPECT_NEAR(beta(50.25, .25), 1.3643, 1e-4);
	EXPECT_NEAR(beta(64, .5), 0.2219, 1e-4);
	EXPECT_NEAR(beta(6400, .5), 0.02215, 1e-4);
}

TEST(binomial_pdf, basic) {
	// matches mma PDF[BinomialDistribution[...], x]
	EXPECT_NEAR(binomial_pdf(40, 0.5, 20), 0.1253, 1e-4);
	EXPECT_NEAR(binomial_pdf(1, 0, 0), 1, 1e-4);
	EXPECT_NEAR(binomial_pdf(1, 1, 0), 0, 1e-4);

	EXPECT_NEAR(binomial_pdf(32, 0.5, 10), 0.01502, 1e-4);
	EXPECT_NEAR(binomial_pdf(32, 0.5, 11), 0.03004, 1e-4);
	EXPECT_NEAR(binomial_pdf(32, 0.5, 12), 0.05257, 1e-4);
	EXPECT_NEAR(binomial_pdf(32, 0.5, 13), 0.08087, 1e-4);
	EXPECT_NEAR(binomial_pdf(32, 0.5, 14), 0.1097, 1e-4);
	EXPECT_NEAR(binomial_pdf(32, 0.5, 15), 0.1317, 1e-4);
	EXPECT_NEAR(binomial_pdf(32, 0.5, 16), 0.1399, 1e-4);
	EXPECT_NEAR(binomial_pdf(32, 0.5, 17), 0.1317, 1e-4);
	EXPECT_NEAR(binomial_pdf(32, 0.5, 18), 0.1097, 1e-4);
	EXPECT_NEAR(binomial_pdf(32, 0.5, 19), 0.08087, 1e-4);
	EXPECT_NEAR(binomial_pdf(32, 0.5, 20), 0.05257, 1e-4);
	EXPECT_NEAR(binomial_pdf(32, 0.5, 21), 0.03004, 1e-4);
	EXPECT_NEAR(binomial_pdf(32, 0.5, 22), 0.01502, 1e-4);

}

TEST(negative_binomial_pdf, basic) {
	// matches mma PDF[NegativeBinomialDistribution[...], x]
	EXPECT_NEAR(negative_binomial_pdf(1, 0, 0), 0, 1e-4);
	EXPECT_NEAR(negative_binomial_pdf(1, 1, 0), 1, 1e-4);
	EXPECT_NEAR(negative_binomial_pdf(10, 0.5, 4), 0.0436, 1e-4);
}


TEST(students_t_cdf, basic) {
	// does not match mma CDF[StudentTDistribution[...], x]
	EXPECT_NEAR(student_t_cdf(1, 5), 0.3632, 1e-4);
	EXPECT_NEAR(student_t_cdf(1.1, 120), 0.2735, 1e-4);
	EXPECT_NEAR(student_t_cdf(0.615227, 4), 0.571683, 1e-4); // matches mma TTest (given t-score)
}

TEST(f_distribution_cdf, basic) {
	EXPECT_NEAR(f_distribution_cdf(0.035082, 4, 4), 0.0033683, 1e-4); // matches mma FTest (given f-score)
}

TEST(chi2_distribution_cdf, basic) {
	// https://en.wikipedia.org/wiki/Chi-squared_distribution#Computational_methods
	// matches mma 1-CDF[ChiSquareDistribution[...], x]
	EXPECT_NEAR(chi2_distribution_cdf(0.1, 5), 1-0.0001623, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(10.1, 5), 1-0.9275, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(13.1, 3.3), 1-0.994, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(3.71429, 4), 1-0.5539, 1e-4);

	EXPECT_NEAR(chi2_distribution_cdf(0, 5), 1-0, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(0, 0), 0, 1e-4);

	EXPECT_NEAR(chi2_distribution_cdf(2008, 200000), 1, 1e-4);
}

TEST(kolmogorov_smirnov_cdf, basic) {
	// there are links to ks cdf implementations here: https://en.wikipedia.org/wiki/Kolmogorov%E2%80%93Smirnov_test#Kolmogorov_distribution 
	EXPECT_NEAR(kolmogorov_smirnov_cdf(0.4, 5, 1000), 0.3128, 1e-4); // boost have a theta function maybe use that (doesn't match mma right now 0.8095)
	// mma: 0.293669
	EXPECT_NEAR(kolmogorov_smirnov_cdf(0.135124, 50, 1000), 0.29916, 1e-4);

	EXPECT_NEAR(kolmogorov_smirnov_cdf(0.135124, 200000, 1000), 0, 1e-4);
}

TEST(kolmogorov_smirnov, basic) {
	// there are links to ks cdf implementations here: https://en.wikipedia.org/wiki/Kolmogorov%E2%80%93Smirnov_test#Kolmogorov_distribution 
	EXPECT_NEAR(kolmogorov_smirnov(0.4, 5), 0.6890, 1e-4); // boost have a theta function maybe use that (doesn't match mma right now 0.8095)
	EXPECT_NEAR(kolmogorov_smirnov2(0.4, 5), 0.6890, 1e-4); // boost have a theta function maybe use that (doesn't match mma right now 0.8095)
}

}
