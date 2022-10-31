#pragma once

#include "util/math.h"

namespace mixer {

inline double t_test(
	double a_n, double a_mean, double a_variance,
	double b_n, double b_mean, double b_variance) {
	const double df = a_n + b_n - 2;
	const double var = ((a_n - 1) * a_variance + (b_n - 1) * b_variance) / df;
	const double t = (a_mean - b_mean) / std::sqrt(var * (1. / a_n + 1. / b_n));
	const double x = (t + sqrt(t * t + df)) / (2.0 * sqrt(t * t + df));
	return incomplete_beta(df / 2.0, df / 2.0, x);
}

inline double z_test(double n, double mean, double population_mean) {
	const auto se = population_mean / std::sqrt(n);
	const auto z = (mean - population_mean) / se;
	return 2. * normal_cdf(-std::abs(z));
}

inline double f_test(double a_n, double a_variance, double b_n, double b_variance) {
	const auto a_nom = a_variance > b_variance;
	const auto f = a_nom ? a_variance / b_variance : b_variance / a_variance;
	const auto df1 = a_nom ? a_n - 1 : b_n - 1;
	const auto df2 = a_nom ? b_n - 1 : a_n - 1;
	return 2. * incomplete_beta(0.5 * df2, 0.5 * df1, df2 / (df2 + df1 * f));
}

}
