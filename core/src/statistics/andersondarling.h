#pragma once

#include <vector>

#include "distributions.h"
#include "streams.h"
#include "types.h"
#include "util/algo.h"

namespace mixer {

inline double anderson_darling(std::vector<double> normalized_data) {
	std::sort(normalized_data.begin(), normalized_data.end());
	double sum = 0;
	for (std::size_t i = 0; i < normalized_data.size(); ++i) {
		assertion(normalized_data[i] > 0 && normalized_data[i] < 1, "normalized data no normalized?");
		sum += (2. * i + 1.) * (
			std::log(normalized_data[i]) +
			std::log(1. - normalized_data[normalized_data.size() - 1 - i]));
	}

	const auto n = static_cast<double>(normalized_data.size());
	sum /= n;

	const double A2 = -n - sum;
	return A2;
}

inline std::vector<statistic> anderson_darling_test(const uint64_t n, const stream& stream) {
	const auto A2 = anderson_darling(get_normalized64(n, stream));
	return {{s_type::anderson_darling, A2, anderson_darling_cdf(A2, n - 1)}};
}

}
