#pragma once

#include <vector>

#include "distributions.h"
#include "types.h"
#include "util/algo.h"

namespace mixer {

inline double anderson_darling_stats(std::vector<double> data01) {
	std::ranges::sort(data01);
	double sum = 0;
	for (std::size_t i = 0; i < data01.size(); ++i) {
		assertion(is_valid_between_01(data01[i]), "anderson darling data not 0-1 or invalid");
		sum += (2. * i + 1.) * (
			std::log(data01[i]) +
			std::log(1. - data01[data01.size() - 1 - i]));
	}

	const auto n = static_cast<double>(data01.size());
	return -n - sum / n;
}

inline std::vector<statistic> anderson_darling_test(const uint64_t n, const stream& stream) {
	const auto A2 = anderson_darling_stats(rescale64_to_01(n, stream));
	return {{s_type::anderson_darling, A2, anderson_darling_cdf(A2, n - 1)}};
}

}
