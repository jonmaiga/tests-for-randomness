#pragma once

#include <cmath>

namespace mixer {

double incomplete_beta(double a, double b, double x);
double gamma_q(double s, double z);

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
	return incomplete_beta(.5 * df, .5, xt);
}

double kolmogorov_smirnov_probability(double D, double N, int conv);

}
