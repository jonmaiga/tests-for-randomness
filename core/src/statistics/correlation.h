#pragma once

#include <vector>
#include "distributions.h"
#include "util/algo.h"
#include "stream.h"

namespace mixer {

struct correlation_stats {
	double pearson_r = 0;
	double spearman_r = 0;
	double kendall_t = 0;
};

inline double adjust_correlation(double d) {
	if (d >= -1 && d <= 1) return d;
	if (is_near(d, -1)) return -1;
	if (is_near(d, 1)) return 1;
	assertion_2(false, "correlation is not within expected range:", std::to_string(d).c_str());
	return d;
}

inline double correlation_p_value(double r, double n) {
	const double t = std::abs(r * std::sqrt((n - 2) / (1 - r * r)));
	return student_t_cdf(t, n - 2);
}

inline std::optional<statistic> pearson_correlation_stats(const std::vector<double>& xs, const std::vector<double>& ys) {
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
	if (is_near(sum_xs, 0) || is_near(sum_ys, 0)) {
		return statistic{statistic_type::pearson_r, 0, sum_xy < 0 ? -1. : 1.};
	}
	const auto r = adjust_correlation(sum_xy / (std::sqrt(sum_xs) * std::sqrt(sum_ys)));
	return statistic{statistic_type::pearson_r, r, correlation_p_value(r, n)};
}

inline std::optional<statistic> spearman_correlation_stats(const std::vector<double>& xs, const std::vector<double>& ys) {
	// @attn get_ranks does not handle non-unique data
	const auto cmp = [](double a, double b) { return a < b; };
	const auto pearson_stats = pearson_correlation_stats(
		rescale_to_01(get_ranks(xs, cmp)),
		rescale_to_01(get_ranks(ys, cmp)));

	return statistic{statistic_type::spearman_r, pearson_stats->value, pearson_stats->p_value};
}

inline std::optional<statistic> kendall_correlation_stats(const std::vector<double>& xs, const std::vector<double>& ys) {
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
	if (n1 == 0 || n2 == 0) {
		return {};
	}
	const auto tau = adjust_correlation(static_cast<double>(is) / (std::sqrt(n1) * std::sqrt(n2)));
	const auto var = ((4. * n + 10.) / (9. * n * (n - 1.)));
	const auto z = tau / std::sqrt(var);
	const auto p_value = normal_two_tailed_cdf(z);
	return statistic{statistic_type::kendall_tau, tau, p_value};
}

inline std::optional<statistic> pearson_correlation_test(uint64_t n, const stream_uint64& source) {
	const auto data = create_serial_xy(n, source);
	return pearson_correlation_stats(data.xs, data.ys);
}

inline std::optional<statistic> spearman_correlation_test(uint64_t n, const stream_uint64& source) {
	const auto data = create_serial_xy(n, source);
	return spearman_correlation_stats(data.xs, data.ys);
}

inline std::optional<statistic> kendall_correlation_test(uint64_t n, const stream_uint64& source) {
	const auto data = create_serial_xy(n, source);
	return kendall_correlation_stats(data.xs, data.ys);
}

}
