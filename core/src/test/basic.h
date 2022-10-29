#pragma once

#include "streams.h"
#include "util/algo.h"

namespace mixer {

struct basic_stats {
	double mean{};
	double variance{};
	double median{};
};

struct basic_result {
	std::string stream_name;
	std::string mixer_name;
	uint64_t n = 0;
	basic_stats stats;
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

inline basic_result basic_test(uint64_t n, const stream& stream, const mixer& mixer) {
	return {
		stream.name,
		mixer.name,
		n,
		compute_basic_stats(n,
		                    get_normalized(n, create_stream_from_mixer(stream, mixer)))
	};
}

}
