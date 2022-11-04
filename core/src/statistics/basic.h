#pragma once

#include "streams.h"
#include "tests.h"
#include "util/algo.h"


namespace mixer {

template <typename T>
double get_median(const std::vector<T>& sorted) {
	assertion(!sorted.empty(), "median in empty vector");
	const auto s = sorted.size();
	if (s % 2 == 1) {
		return sorted[s / 2];
	}
	return (sorted[(s - 1) / 2] + sorted[(s - 1) / 2 + 1]) / 2.;
}

template <typename T>
double get_mean(const std::vector<T>& values) {
	assertion(!values.empty(), "mean in empty vector");
	double sum = 0;
	for (const auto v : values) {
		sum += v;
	}
	return sum / static_cast<double>(values.size());
}

template <typename T>
double get_variance(const std::vector<T>& values, double mean) {
	assertion(values.size() > 1, "at least two elements required to calculate variance");
	double sum = 0;
	for (const auto v : values) {
		sum += (v - mean) * (v - mean);
	}
	return sum / static_cast<double>(values.size() - 1);
}

struct basic_stats {
	double n{};
	double mean{};
	double variance{};
};

inline basic_stats compute_basic_stats(const std::vector<double>& values) {
	basic_stats stats{static_cast<double>(values.size())};
	stats.mean = get_mean(values);
	stats.variance = get_variance(values, stats.mean);
	return stats;
}

inline std::vector<statistic> basic_test(uint64_t n, const stream& stream) {
	// mean from uniform is approximately normal
	// https://stats.stackexchange.com/questions/458341/what-distribution-does-the-mean-of-a-random-sample-from-a-uniform-distribution-f
	const auto ns = get_normalized64(n, stream);
	const auto stats = compute_basic_stats(ns);

	// not sure what to do with variance, can't get p values uniformly distributed.
	// https://www.youtube.com/watch?v=V4Rm4UQHij0 sampling dist of variance is not normal!
	// We can't use this for variance since it's sampling is non normal!
	// https://stats.stackexchange.com/questions/121662/why-is-the-sampling-distribution-of-variance-a-chi-squared-distribution
	//constexpr auto pop_var = 1. / 12.;
	//const auto var_x2 = (n - 1) * stats.variance / pop_var;
	//const auto p_value = chi2_distribution_normal_approximation_cdf(var_x2, n - 1);
	//const auto p_value = f_test(stats.n, stats.variance, population_variance);
	//const auto pop_var_var = 2 * pop_var * pop_var / (n - 1);
	//const auto p_value = z_test(stats.n, stats.variance, pop_var, pop_var_var);

	return {
		{s_type::basic_mean, stats.mean, z_test(stats.n, stats.mean, .5, 1. / 12.)},
		{s_type::basic_variance, stats.variance}
	};
}


}
