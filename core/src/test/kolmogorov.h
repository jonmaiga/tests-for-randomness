#pragma once

#include <vector>

#include "streams.h"
#include "types.h"
#include "util/algo.h"

namespace mixer {

struct kolmogorov_stats {
	double d_max{};
	uint64_t i_max{};
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

inline kolmogorov_stats kolmogorov_test(const uint64_t n, const stream& stream, const mixer& mixer) {
	const auto mixer_stream = create_stream_from_mixer(stream, mixer);
	return kolmogorov_smirnov_test(get_normalized(n, stream));
}

}
