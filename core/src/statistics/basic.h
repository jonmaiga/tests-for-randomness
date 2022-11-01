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


struct basic_stats {
	double n{};
	double mean{};
	double variance{};
};

inline basic_stats compute_basic_stats(const std::vector<double>& values) {
	basic_stats stats{static_cast<double>(values.size())};
	stats.mean /= get_mean(values);
	for (const auto v : values) {
		stats.variance += (v - stats.mean) * (v - stats.mean);
	}
	stats.variance /= stats.n;
	return stats;
}

inline std::vector<statistic> basic_test(uint64_t n, const stream& stream) {
	// mean from uniform is approximately normal
	// https://stats.stackexchange.com/questions/458341/what-distribution-does-the-mean-of-a-random-sample-from-a-uniform-distribution-f

	// not sure what to do with variance, for large sample sizes it may be enough with z-test
	// p-values for z_test(stats.n, stats.variance, 1./15., 1./15.) looks normally distributed

	const auto ns = get_normalized64(n, stream);
	const auto stats = compute_basic_stats(ns);
	return {
		{s_type::basic_mean, stats.mean, z_test(stats.n, stats.mean, .5, 1. / 12.)},
		{s_type::basic_variance, stats.variance}
	};
}


}
