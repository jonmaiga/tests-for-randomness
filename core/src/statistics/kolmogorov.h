#pragma once

#include <vector>

#include "distributions.h"
#include "types.h"
#include "util/algo.h"

namespace mixer {

inline std::optional<statistic> kolmogorov_smirnov_stats(std::vector<double> data01) {
	std::sort(data01.begin(), data01.end());
	const auto n = static_cast<double>(data01.size());
	double max_distance = 0;
	for (std::size_t i = 0; i < data01.size(); ++i) {
		const double e0 = i / n;
		const double e1 = (i + 1) / n;
		const double d = data01[i];
		const double distance = std::max(std::abs(e0 - d), std::abs(e1 - d));
		max_distance = std::max(distance, max_distance);
	}
	const auto p_value = kolmogorov_smirnov_cdf(max_distance, n, 100);
	return statistic{statistic_type::kolmogorov_smirnov_d, max_distance, p_value};
}

inline std::optional<statistic> kolmogorov_test(const uint64_t n, const stream_uint64& stream) {
	return kolmogorov_smirnov_stats(rescale64_to_01(n, stream));
}

}
