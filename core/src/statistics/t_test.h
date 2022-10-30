#pragma once

namespace mixer {

inline double normal_cdf(double x) {
	return std::erfc(-x / std::sqrt(2)) / 2;
}

inline basic_stats get_uniform_stats(double n) {
	return {n, 0.5, 1. / 12.};
}

inline double t_test(basic_stats a, basic_stats b) {
	const double df = a.n + b.n - 2;
	const double var = ((a.n - 1) * a.variance + (b.n - 1) * b.variance) / df;
	const double t = (a.mean - b.mean) / std::sqrt(var * (1. / a.n + 1. / b.n));
	const double x = (t + sqrt(t * t + df)) / (2.0 * sqrt(t * t + df));
	return incomplete_beta(df / 2.0, df / 2.0, x);
}

inline double z_test(basic_stats a, basic_stats population) {
	const auto se = population.mean / std::sqrt(a.n);
	const auto z = (a.mean - population.mean) / se;
	return 2. * normal_cdf(-std::abs(z));
}


}
