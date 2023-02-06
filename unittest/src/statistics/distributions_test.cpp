#include <statistics/distributions.h>

#include <gtest/gtest.h>

namespace tfr {

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

	EXPECT_NEAR(binomial_pdf(64, 0.5, 31), 0.09633, 1e-4);
	EXPECT_NEAR(binomial_pdf(64, 0.5, 32), 0.09934, 1e-4);
	EXPECT_NEAR(binomial_pdf(64, 0.5, 33), 0.09633, 1e-4);
}

TEST(flip_coin_pdf, basic) {
	// matches mma PDF[BinomialDistribution[...], x]
	EXPECT_NEAR(flip_coin_pdf(40, 20), 0.1253, 1e-4);

	EXPECT_NEAR(flip_coin_pdf(32, 10), 0.01502, 1e-4);
	EXPECT_NEAR(flip_coin_pdf(32, 11), 0.03004, 1e-4);
	EXPECT_NEAR(flip_coin_pdf(32, 12), 0.05257, 1e-4);
	EXPECT_NEAR(flip_coin_pdf(32, 13), 0.08087, 1e-4);
	EXPECT_NEAR(flip_coin_pdf(32, 14), 0.1097, 1e-4);
	EXPECT_NEAR(flip_coin_pdf(32, 15), 0.1317, 1e-4);
	EXPECT_NEAR(flip_coin_pdf(32, 16), 0.1399, 1e-4);
	EXPECT_NEAR(flip_coin_pdf(32, 17), 0.1317, 1e-4);
	EXPECT_NEAR(flip_coin_pdf(32, 18), 0.1097, 1e-4);
	EXPECT_NEAR(flip_coin_pdf(32, 19), 0.08087, 1e-4);
	EXPECT_NEAR(flip_coin_pdf(32, 20), 0.05257, 1e-4);
	EXPECT_NEAR(flip_coin_pdf(32, 21), 0.03004, 1e-4);
	EXPECT_NEAR(flip_coin_pdf(32, 22), 0.01502, 1e-4);

	EXPECT_NEAR(flip_coin_pdf(64, 31), 0.09633, 1e-4);
	EXPECT_NEAR(flip_coin_pdf(64, 32), 0.09934, 1e-4);
	EXPECT_NEAR(flip_coin_pdf(64, 33), 0.09633, 1e-4);
}

TEST(flip_coin_pdf, accuracy) {
	const std::vector<double> mma = {
		2.328306436538698e-10, 7.450580596923824e-9,
		1.1548399925231957e-7, 1.154839992523197e-6,
		8.37258994579318e-6, 0.0000468865036964417,
		0.00021098926663398775, 0.0007836744189262385, 0.002448982559144491,
		0.006530620157718642, 0.015020426362752953, 0.030040852725505853,
		0.052571492269635034, 0.08087921887636176, 0.1097646541893478,
		0.131717585027218, 0.13994993409141898, 0.131717585027218,
		0.1097646541893478, 0.08087921887636176, 0.052571492269635034,
		0.030040852725505853, 0.015020426362752953, 0.006530620157718642,
		0.002448982559144491, 0.0007836744189262385, 0.00021098926663398775,
		0.0000468865036964417, 8.372589945793167e-6,
		1.154839992523197e-6, 1.1548399925231957e-7,
		7.450580596923824e-9, 2.328306436538698e-10
	};

	for (auto k = 0; k <= 32; ++k) {
		EXPECT_NEAR(binomial_pdf(32, .5, k), mma[k], 1e-14);
	}
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

	EXPECT_NEAR(chi2_distribution_cdf(9, 10), 0.5321, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(10, 10), 0.4404, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(11, 10), 0.3575, 1e-4);

	EXPECT_NEAR(chi2_distribution_cdf(90, 100), 0.7531, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(99, 100), 0.5094, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(100, 100), 0.4811, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(101, 100), 0.4531, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(110, 100), 0.2322, 1e-4);

	EXPECT_NEAR(chi2_distribution_cdf(490, 500), 0.6168, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(499, 500), 0.5042, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(500, 500), 0.4915, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(501, 500), 0.4789, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(510, 500), 0.3687, 1e-4);

	EXPECT_NEAR(chi2_distribution_cdf(990, 1000), 0.5829, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(999, 1000), 0.5029, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(1000, 1000), 0.4940, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(1001, 1000), 0.4851, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(1010, 1000), 0.4060, 1e-4);

	EXPECT_NEAR(chi2_distribution_cdf(1990, 2000), 0.5587, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(2990, 3000), 0.5480, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(4990, 5000), 0.5372, 1e-4);

	EXPECT_NEAR(chi2_distribution_cdf(9990, 10000), 0.5263, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(10000, 10000), 0.4981, 1e-4);
	EXPECT_NEAR(chi2_distribution_cdf(10010, 10000), 0.4699, 1e-4);

	EXPECT_NEAR(chi2_distribution_cdf(999900, 1000000), 0.5280, 1e-4); // 0.5279 mma
	EXPECT_NEAR(chi2_distribution_cdf(1000000, 1000000), 0.4998, 1e-4); // 0.4998 mma
	EXPECT_NEAR(chi2_distribution_cdf(1000100, 1000000), 0.4716, 1e-4); // 0.4716 mma
}

TEST(chi2_distribution_cdf, uses_approximation) {
	EXPECT_NEAR(chi2_distribution_cdf(1999900, 2000000), 0.5198, 1e-4); // 0.5198 mma
	EXPECT_NEAR(chi2_distribution_cdf(2000000, 2000000), 0.4999, 1e-4); // 0.4998 mma
	EXPECT_NEAR(chi2_distribution_cdf(2000100, 2000000), 0.4799, 1e-4); // 0.4799 mma
}

TEST(chi2_distribution_cdf, approximation_large_1) {
	EXPECT_NEAR(chi2_distribution_normal_approximation_cdf(9990, 10000), 0.5267, 1e-4); // mma 0.5263
	EXPECT_NEAR(chi2_distribution_normal_approximation_cdf(10000, 10000), 0.4985, 1e-4); // mma 0.4981
	EXPECT_NEAR(chi2_distribution_normal_approximation_cdf(10010, 10000), 0.4704, 1e-4); // mma 0.4699
}

TEST(chi2_distribution_cdf, approximation_large_2) {
	EXPECT_NEAR(chi2_distribution_normal_approximation_cdf(32437.3, 32767), 0.9014, 1e-4);
	EXPECT_NEAR(chi2_distribution_normal_approximation_cdf(32600, 32767), 0.7425, 1e-4);
	EXPECT_NEAR(chi2_distribution_normal_approximation_cdf(32767, 32767), 0.4992, 1e-4);
	EXPECT_NEAR(chi2_distribution_normal_approximation_cdf(32800, 32767), 0.4479, 1e-4);
}

TEST(chi2_distribution_cdf, approximation_large_3) {
	EXPECT_NEAR(chi2_distribution_normal_approximation_cdf(31500, 32000), 0.9762, 1e-4); // 0.9763 mma
	EXPECT_NEAR(chi2_distribution_normal_approximation_cdf(32000, 32000), 0.4992, 1e-4); // 0.4989 mma
	EXPECT_NEAR(chi2_distribution_normal_approximation_cdf(32500, 32000), 0.02437, 1e-4); // 0.02448 mma
}

TEST(chi2_distribution_cdf, approximation_large_4) {
	EXPECT_NEAR(chi2_distribution_normal_approximation_cdf(999900, 1000000), 0.5280, 1e-4); // 0.5279 mma
	EXPECT_NEAR(chi2_distribution_normal_approximation_cdf(1000000, 1000000), 0.4998, 1e-4); // 0.4998 mma
	EXPECT_NEAR(chi2_distribution_normal_approximation_cdf(1000100, 1000000), 0.4716, 1e-4); // 0.0.4716 mma
}

TEST(chi2_distribution_cdf, approximation_large_5) {
	EXPECT_NEAR(chi2_distribution_normal_approximation_cdf(1e12, 1e12), 0.4999, 1e-4); // 0.4991 mma
	EXPECT_NEAR(chi2_distribution_normal_approximation_cdf(1e12-1e6, 1e12), 0.7602, 1e-4); // 0.7606 mma
	EXPECT_NEAR(chi2_distribution_normal_approximation_cdf(1e12+1e6, 1e12), 0.2397, 1e-4); // 0.2400 mma
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
}

}
