#pragma once

#include "streams.h"
#include "tests.h"
#include "util/algo.h"


namespace mixer {

struct basic_stats {
	double n{};
	double mean{};
	double variance{};
};

inline basic_stats compute_basic_stats(const std::vector<double>& values) {
	basic_stats stats{};
	for (const auto v : values) {
		stats.mean += v;
	}
	stats.n = static_cast<double>(values.size());
	stats.mean /= stats.n;

	for (const auto v : values) {
		stats.variance += (v - stats.mean) * (v - stats.mean);
	}
	stats.variance /= stats.n;
	return stats;
}

inline std::vector<statistic> basic_test(uint64_t n, const stream& stream) {
	// mean from uniform is approximately normal
	// https://stats.stackexchange.com/questions/458341/what-distribution-does-the-mean-of-a-random-sample-from-a-uniform-distribution-f
	const auto stats = compute_basic_stats(get_normalized(n, stream));

	return {
		{s_type::basic_mean, stats.mean, z_test(stats.n, stats.mean, .5, 1./12.)},
		{s_type::basic_variance, stats.variance, z_test(stats.n, stats.variance, .5, 1. / 12.)}
	};
}


}
