#pragma once

#include "streams.h"

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

inline basic_stats compute_basic_test(uint64_t n, const stream& stream) {
	constexpr auto normalizer = static_cast<double>(std::numeric_limits<uint64_t>::max());

	basic_stats stats{};
	std::vector<double> values;
	values.reserve(n);
	for (uint64_t i = 0; i < n; ++i) {
		const auto value = static_cast<double>(stream()) / normalizer;
		stats.mean += value;
		values.push_back(value);
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
		compute_basic_test(n, create_stream_from_mixer(stream, mixer))
	};
}

}
