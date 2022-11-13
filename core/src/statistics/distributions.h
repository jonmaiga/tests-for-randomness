#pragma once

#include <cmath>
#include "util/math.h"

namespace mixer {

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
	return 1. - 0.5 * (1 + std::erf((chi2 - df) / (2 * std::sqrt(df))));
}

inline double chi2_distribution_cdf(double chi2, double df) {
	// found here: https://en.wikipedia.org/wiki/Chi-squared_distribution
	if (is_near(df, 0)) { 
		return 0;
	}
	return gamma_regularized(.5 * df, .5 * chi2);
}

inline double beta(double a, double b) {
	return std::beta(a, b);
	// return exp((std::lgamma(a) + std::lgamma(b)) - std::lgamma(a + b));
}

inline double binomial_coefficient(int n, int k) {
	return 1 / ((n + 1) * beta(n - k + 1, k + 1));
}

inline double binomial_pdf(double n, double p, double k) {
	return binomial_coefficient(n, k) * std::pow(p, k) * std::pow((1 - p), n - k);
}

double kolmogorov_smirnov_cdf(double D, double df, int conv);

double anderson_darling_cdf(double A2, double df);

}
