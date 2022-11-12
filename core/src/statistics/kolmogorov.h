#pragma once

#include <vector>

#include "distributions.h"
#include "types.h"
#include "util/algo.h"

namespace mixer {

inline double kolmogorov_smirnov_stats(std::vector<double> data) {
	std::ranges::sort(data);
	const auto n = static_cast<double>(data.size());
	double max_distance = 0;
	for (std::size_t i = 0; i < data.size(); ++i) {
		const double e0 = i / n;
		const double e1 = (i + 1) / n;
		const double d = data[i];
		const double distance = std::max(std::abs(e0 - d), std::abs(e1 - d));
		max_distance = std::max(distance, max_distance);
	}
	return max_distance;
}

inline std::vector<statistic> kolmogorov_test(const uint64_t n, const stream& stream) {
	const auto max_distance = kolmogorov_smirnov_stats(rescale64_to_01(n, stream));
	const auto p_value = kolmogorov_smirnov_cdf(max_distance, n, 100);
	return {{s_type::kolmogorov_smirnov, max_distance, p_value}};
}

}
