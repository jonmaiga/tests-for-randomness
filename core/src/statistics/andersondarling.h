#pragma once

#include <algorithm>
#include <vector>

#include "distributions.h"
#include "types.h"
#include "util/algo.h"

namespace mixer {

inline double anderson_darling_stats(std::vector<double> data01) {
	std::sort(data01.begin(), data01.end());
	double sum = 0;
	for (std::size_t i = 0; i < data01.size(); ++i) {
		assertion(is_valid_between_01(data01[i]), "anderson darling data not 0-1 or invalid");
		const double a = data01[i];
		const double b = data01[data01.size() - 1 - i];
		sum += (2. * i + 1.) * (log_safe(a) + log_safe(1. - b));
	}

	const auto n = static_cast<double>(data01.size());
	return -n - sum / n;
}

inline std::optional<statistic> anderson_darling_test(const uint64_t n, const stream_uint64& stream) {
	const auto A2 = anderson_darling_stats(rescale64_to_01(n, stream));
	return statistic{test_type::anderson_darling, A2, anderson_darling_cdf(A2, n - 1)};
}

}
