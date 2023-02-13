#pragma once

#include <cmath>
#include "util/math.h"

namespace tfr {

double beta_regularized(double a, double b, double x);
double gamma_regularized(double s, double z);

inline double normal_cdf(double z) {
	static const double sqrt_2 = std::sqrt(2);
	return std::erfc(-z / sqrt_2) / 2.;
}

inline double normal_two_tailed_cdf(double z) {
	return 2. * normal_cdf(-std::abs(z));
}

inline double student_t_cdf(double t, double df) {
	// found here: https://en.wikipedia.org/wiki/Student%27s_t-distribution#Integral_of_Student's_probability_density_function_and_p-value
	const double xt = df / (df + t * t);
	return beta_regularized(.5 * df, .5, xt);
}

inline double f_distribution_cdf(double f, double d1, double d2) {
	// found here: https://en.wikipedia.org/wiki/F-distribution
	const double xt = d1 * f / (d1 * f + d2);
	return beta_regularized(.5 * d1, .5 * d2, xt);
}

inline double chi2_distribution_normal_approximation_cdf(double chi2, double df) {
	return 1 - normal_cdf(std::sqrt(2 * chi2) - std::sqrt(2 * df - 1));
}

inline double chi2_distribution_cdf(double chi2, double df) {
	// found here: https://en.wikipedia.org/wiki/Chi-squared_distribution
	if (is_near(df, 0)) {
		return 0;
	}
	if (df >= 1100000) {
		return chi2_distribution_normal_approximation_cdf(chi2, df);
	}
	return gamma_regularized(.5 * df, .5 * chi2);
}

inline double beta(double a, double b) {
	// return std::beta(a, b);
	return exp(log_gamma(a) + log_gamma(b) - log_gamma(a + b));
}

inline double binomial_coefficient(int n, int k) {
	return 1 / ((n + 1) * beta(n - k + 1, k + 1));
}

inline uint64_t bin_c(uint64_t n, uint64_t k) {
	double res = 1;
	for (int i = 1; i <= k; ++i)
		res = res * (n - k + i) / i;
	return static_cast<uint64_t>(std::round(res));
}

inline double binomial_pdf(uint64_t n, double p, uint64_t k) {
	return binomial_coefficient(n, k) * std::pow(p, k) * std::pow((1 - p), n - k);
}

inline double negative_binomial_pdf(uint64_t successes, double p, uint64_t failures) {
	return binomial_coefficient(failures + successes - 1, successes - 1) *
		std::pow(1 - p, failures) *
		std::pow(p, successes);
}

inline double flip_coin_pdf(uint64_t n, uint64_t k) {
	const auto C = bin_c(n, k);
	const auto p = std::pow(2, n);
	return C / p;
}

double kolmogorov_smirnov_cdf(double D, double df, int conv);

double anderson_darling_cdf(double A2, double df);

inline double kolmogorov_smirnov(double D, double df) {
	using namespace constants;
	double sum = 0;
	double x = std::sqrt(df) * D;
	x = x + 1. / (6. * std::sqrt(df)) + (x - 1) / (4 * df);
	constexpr double c = PiSqr / 8.;
	const double cx = c / (x * x);
	for (uint64_t k = 1; k <= 1000; ++k) {
		auto t = static_cast<double>(2 * k - 1);
		t *= t;
		t *= cx;
		sum += std::exp(-t);
	}
	sum *= std::sqrt(2. * Pi) / x;
	return sum;
}

}
