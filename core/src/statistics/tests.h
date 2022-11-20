#pragma once

#include "distributions.h"
#include "util/math.h"

namespace mixer {

inline double t_test(
	double a_n, double a_mean, double a_variance,
	double b_n, double b_mean, double b_variance) {
	const double df = a_n + b_n - 2;
	const double var = ((a_n - 1) * a_variance + (b_n - 1) * b_variance) / df;
	const double t = (a_mean - b_mean) / std::sqrt(var * (1. / a_n + 1. / b_n));
	return student_t_cdf(t, df);
}

inline double t_test(double n,
                     double a_mean, double a_variance,
                     double b_mean, double b_variance) {
	return t_test(n, a_mean, a_variance, n, b_mean, b_variance);
}

inline std::optional<statistic> z_test(double n, double mean, double population_mean, double population_variance) {
	if (is_near(population_variance, 0)) {
		return {};
	}
	if (is_near(n, 0)) {
		return {};
	}
	const auto se = std::sqrt(population_variance / n);
	const auto z = (mean - population_mean) / se;
	// todo: df
	return statistic{statistic_type::z_score, z, normal_two_tailed_cdf(z), n};
}

inline double f_test(double a_n, double a_variance, double b_n, double b_variance) {
	const auto a_nom = a_variance > b_variance;
	const auto f = a_nom ? a_variance / b_variance : b_variance / a_variance;
	const auto df1 = (a_nom ? a_n : b_n) - 1;
	const auto df2 = (a_nom ? b_n : a_n) - 1;
	const auto x = df2 / (df2 + df1 * f);
	return 2. * beta_regularized(0.5 * df2, 0.5 * df1, x);
}

inline double f_test(double n, double a_variance, double b_variance) {
	return f_test(n, a_variance, n, b_variance);
}

}
