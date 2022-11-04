#pragma once

#include <vector>
#include "distributions.h"
#include "util/algo.h"

namespace mixer {

struct correlation_stats {
	double pearson_r = 0;
	double spearman_r = 0;
	double kendall_t = 0;
};

inline double pearson_correlation(const std::vector<double>& xs, const std::vector<double>& ys) {
	const auto n = xs.size();
	double x_mean = 0, y_mean = 0;
	for (uint64_t i = 0; i < n; ++i) {
		x_mean += xs[i];
		y_mean += ys[i];
	}
	x_mean /= n;
	y_mean /= n;

	double sum_xs = 0;
	double sum_ys = 0;
	double sum_xy = 0;
	for (std::size_t i = 0; i < n; ++i) {
		const double x_dev = xs[i] - x_mean;
		const double y_dev = ys[i] - y_mean;
		sum_xs += x_dev * x_dev;
		sum_ys += y_dev * y_dev;
		sum_xy += x_dev * y_dev;
	}
	return sum_xy / (std::sqrt(sum_xs) * std::sqrt(sum_ys));
}

inline double spearman_correlation(const std::vector<double>& xs, const std::vector<double>& ys) {
	// @attn get_ranks does not handle non-unique data
	const auto cmp = [](double a, double b) { return a < b; };
	return pearson_correlation(
		normalize_to_uniform(get_ranks(xs, cmp)),
		normalize_to_uniform(get_ranks(ys, cmp)));
}

inline double kendall_correlation(const std::vector<double>& xs, const std::vector<double>& ys) {
	const size_t n = xs.size();
	uint64_t n1 = 0, n2 = 0;
	int64_t is = 0;
	for (size_t j = 0; j < n - 1; ++j) {
		for (size_t k = j + 1; k < n; ++k) {
			const double diff_x = xs[j] - xs[k];
			const double diff_y = ys[j] - ys[k];
			const double xy = diff_x * diff_y;
			if (xy != 0.) {
				++n1;
				++n2;
				xy > 0. ? ++is : --is;
			}
			else {
				if (diff_x != 0.) ++n1;
				if (diff_y != 0.) ++n2;
			}
		}
	}
	return static_cast<double>(is) / (std::sqrt(n1) * std::sqrt(n2));
}

inline double correlation_p_value(double r, double n) {
	const double t = std::abs(r * std::sqrt((n - 2) / (1 - r * r)));
	return student_t_cdf(t, n - 2);
}

inline std::vector<statistic> pearson_correlation_mixer_test(uint64_t n, const stream& source, const mixer& mixer) {
	const auto data = create_bit_flipped_xy(n, source, mixer);
	const auto correlation = pearson_correlation(data.xs, data.ys);
	const auto p_value = correlation_p_value(correlation, data.xs.size());
	return {{s_type::pearson_r, correlation, p_value}};
}

inline std::vector<statistic> spearman_correlation_mixer_test(uint64_t n, const stream& source, const mixer& mixer) {
	const auto data = create_bit_flipped_xy(n, source, mixer);
	const auto rho = spearman_correlation(data.xs, data.ys);
	const auto p_value = correlation_p_value(rho, data.xs.size());
	return {{s_type::spearman_r, rho, p_value}};
}

inline std::vector<statistic> kendall_correlation_mixer_test(uint64_t n, const stream& source, const mixer& mixer) {
	const auto data = create_bit_flipped_xy(n, source, mixer);
	const auto tau = kendall_correlation(data.xs, data.ys);
	const auto p_value = correlation_p_value(tau, data.xs.size());
	return {{s_type::kendall_tau, tau, p_value}};
}

}
