#pragma once

#include <cmath>

namespace mixer {

double beta_regularized(double a, double b, double x);
double gamma_regularized(double s, double z);

inline double normal_cdf(double z) {
	static const double sqrt_2 = std::sqrt(2);
	return std::erfc(-z / sqrt_2) / 2.;
}

inline double two_tailed_normal_cdf(double z) {
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

inline double chi2_distribution_cdf(double chi2, double df) {
	// found here: https://en.wikipedia.org/wiki/Chi-squared_distribution
	return 1. - gamma_regularized(.5 * df, .5 * chi2);
}

double kolmogorov_smirnov_cdf(double D, double df, int conv);

double anderson_darling_cdf(double A2, double df);

}
