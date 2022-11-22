#pragma once

#include "streams.h"
#include "tests.h"
#include "util/algo.h"


namespace mixer {

template <typename T>
double get_median(const T& sorted) {
	assertion(!sorted.empty(), "median in empty vector");
	const auto s = sorted.size();
	if (s % 2 == 1) {
		return sorted[s / 2];
	}
	return (sorted[(s - 1) / 2] + sorted[(s - 1) / 2 + 1]) / 2.;
}

template <typename T>
double get_mean(const T& values) {
	double sum = 0;
	for (const auto v : values) {
		sum += v;
	}
	return sum / static_cast<double>(values.size());
}

template <typename T>
double get_sum_of_squares(const T& values, double mean) {
	assertion(values.size() > 1, "at least two elements required to calculate variance");
	double sum = 0;
	for (const auto v : values) {
		sum += (v - mean) * (v - mean);
	}
	return sum;
}

struct basic_statistics {
	double n{};
	double mean{};
	double sum_of_squares{};
	double variance() const { return sum_of_squares / n; }
	double sample_variance() const { return sum_of_squares / (n - 1); }
};

template <typename T>
basic_statistics basic_stats(const T& values) {
	basic_statistics stats{static_cast<double>(values.size())};
	stats.mean = get_mean(values);
	stats.sum_of_squares = get_sum_of_squares(values, stats.mean);
	return stats;
}

template <typename T>
sub_test_results basic_test(uint64_t n, stream<T> stream) {
	// mean from uniform is approximately normal
	// https://stats.stackexchange.com/questions/458341/what-distribution-does-the-mean-of-a-random-sample-from-a-uniform-distribution-f
	const auto stats = basic_stats(ranged_stream(rescale_type_to_01(stream), n));
	return main_sub_test(z_test(stats.n, stats.mean, .5, 1. / 12.));
}


}
