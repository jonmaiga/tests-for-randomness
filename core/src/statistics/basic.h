#pragma once

#include "streams.h"
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

inline basic_stats basic_test(uint64_t n, const stream& stream) {
	return compute_basic_stats(get_normalized(n, stream));
}


}
