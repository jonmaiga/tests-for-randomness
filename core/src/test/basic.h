#pragma once

#include "streams.h"
#include "util/algo.h"

namespace mixer {

struct basic_stats {
	double mean{};
	double variance{};
	double median{};
};

inline basic_stats compute_basic_stats(uint64_t n, std::vector<double> values) {
	basic_stats stats{};
	for (const auto v : values) {
		stats.mean += v;
	}
	stats.mean /= static_cast<double>(n);

	std::sort(values.begin(), values.end());
	for (const auto v : values) {
		stats.variance += (v - stats.mean) * (v - stats.mean);
	}
	stats.variance /= static_cast<double>(n);
	stats.median = values[values.size() / 2];
	return stats;
}

inline basic_stats basic_test(uint64_t n, const stream& stream, const mixer& mixer) {
	return compute_basic_stats(
		n,
		get_normalized(n, create_stream_from_mixer(stream, mixer)));
}

}
