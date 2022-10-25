#pragma once

#include <string>
#include <vector>

#include "streams.h"
#include "types.h"

namespace mixer {

struct kolmogorov_stats {
	double d_max;
	std::size_t i_max;
};

struct kolmogorov_result {
	std::string stream_name;
	std::string mixer_name;
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

inline kolmogorov_result kolmogorov_test(const stream& stream, const mixer& mixer) {
	const auto mixer_stream = create_mixer_stream(stream, mixer);
	constexpr auto normalizer = static_cast<double>(std::numeric_limits<uint64_t>::max());
	std::vector<double> data;
	try {
		while (true) {
			const auto d = static_cast<double>(mixer_stream());
			data.push_back(d / normalizer);
		}
	}
	catch (const std::runtime_error&) {
	}
	return {stream.name, mixer.name, kolmogorov_smirnov_test(data)};
}

}
