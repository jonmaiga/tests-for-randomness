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
	if (n < 3) {
		return {};
	}
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
		return statistic{statistic_type::z_score, 0, 1, static_cast<double>(n)};
	}

	// from: https://en.wikipedia.org/wiki/Pearson_correlation_coefficient#Using_the_Fisher_transformation
	const auto r = adjust_correlation(sum_xy / (std::sqrt(sum_xs) * std::sqrt(sum_ys)));
	const auto mean = fishers_transformation_safe(r);
	assertion(is_valid(mean), "fishers transform failed");
	const auto z = mean * std::sqrt(n - 3);
	const auto p = normal_two_tailed_cdf(z);
	return statistic(statistic_type::z_score, z, p, n);
}

inline std::optional<statistic> spearman_correlation_stats(const std::vector<double>& xs, const std::vector<double>& ys) {
	// from: https://en.wikipedia.org/wiki/Spearman%27s_rank_correlation_coefficient#Determining_significance
	// @attn get_ranks does not handle non-unique data
	// @attn z should be divided by sqrt(1.06)
	const auto cmp = [](double a, double b) { return a < b; };
	return pearson_correlation_stats(
		rescale_to_01(get_ranks(xs, cmp)),
		rescale_to_01(get_ranks(ys, cmp)));
}

inline std::optional<statistic> kendall_correlation_stats(const std::vector<double>& xs, const std::vector<double>& ys) {
	const size_t n = xs.size();
	if (n < 2) {
		return {};
	}

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

	// from https://en.wikipedia.org/wiki/Kendall_rank_correlation_coefficient#Hypothesis_test
	const auto tau = adjust_correlation(static_cast<double>(is) / (std::sqrt(n1) * std::sqrt(n2)));
	const auto var = (4. * n + 10.) / (9. * n * (n - 1.));
	const auto z = tau / std::sqrt(var);
	const auto p_value = normal_two_tailed_cdf(z);
	return statistic{statistic_type::z_score, z, p_value, static_cast<double>(n)};
}

template <typename T>
sub_test_results pearson_correlation_test(uint64_t n, stream<T> source) {
	return split_test(n, 1000000, [&source](uint64_t size) {
		const auto data = create_serial_xy_by_ref(size, source);
		return pearson_correlation_stats(data.xs, data.ys);
	});
}

template <typename T>
sub_test_results spearman_correlation_test(uint64_t n, stream<T> source) {
	return split_test(n, 1000000, [&source](uint64_t size) {
		const auto data = create_serial_xy_by_ref(size, source);
		return spearman_correlation_stats(data.xs, data.ys);
	});
}

template <typename T>
sub_test_results kendall_correlation_test(uint64_t n, stream<T> source) {
	return split_test(n, 10000, [&source](uint64_t size) {
		const auto data = create_serial_xy_by_ref(size, source);
		return kendall_correlation_stats(data.xs, data.ys);
	});
}

}
