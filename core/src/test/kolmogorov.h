#pragma once

#include <string>
#include <vector>

#include "streams.h"
#include "types.h"
#include "util/math.h"

namespace mixer {

struct kolmogorov_stats {
	double d_max{};
	uint64_t i_max{};
};

struct kolmogorov_result {
	std::string stream_name;
	std::string mixer_name;
	uint64_t n{};
	kolmogorov_stats stats;
};

inline kolmogorov_stats kolmogorov_smirnov_test(std::vector<double> data) {
	std::sort(data.begin(), data.end());
	const auto n = static_cast<double>(data.size());
	double max_distance = 0;
	std::size_t max_index = 0;
	for (std::size_t i = 0; i < data.size(); ++i) {
		const double e0 = i / n;
		const double e1 = (i + 1) / n;
		const double d = data[i];
		const double distance = std::max(std::abs(e0 - d), std::abs(e1 - d));
		if (distance > max_distance) {
			max_distance = distance;
			max_index = i;
		}
	}
	return {max_distance, max_index};
}

inline kolmogorov_result kolmogorov_test(const uint64_t n, const stream& stream, const mixer& mixer) {
	const auto mixer_stream = create_stream_from_mixer(stream, mixer);
	std::vector<double> data;
	for (uint64_t i = 0; i < n; ++i) {
		data.push_back(normalize(mixer_stream()));
	}
	return {stream.name, mixer.name, data.size(), kolmogorov_smirnov_test(data)};
}

}
