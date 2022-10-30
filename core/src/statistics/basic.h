#pragma once

#include "streams.h"
#include "util/algo.h"

namespace mixer {

struct basic_stats {
	double mean{};
	double median{};
	double variance{};
	double standard_deviation{};
};

inline basic_stats compute_basic_stats(uint64_t n, std::vector<double> values) {
	basic_stats stats{};
	for (const auto v : values) {
		stats.mean += v;
	}
	stats.mean /= static_cast<double>(n);
	std::sort(values.begin(), values.end());
	stats.median = values[values.size() / 2];

	for (const auto v : values) {
		stats.variance += (v - stats.mean) * (v - stats.mean);
	}
	stats.variance /= static_cast<double>(n);
	stats.standard_deviation = std::sqrt(stats.variance);
	return stats;
}

inline basic_stats basic_test(uint64_t n, const stream& stream) {
	return compute_basic_stats(n, get_normalized(n, stream));
}

}
