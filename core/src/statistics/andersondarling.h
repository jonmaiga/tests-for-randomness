#pragma once

#include <vector>

#include "streams.h"
#include "types.h"
#include "util/algo.h"

namespace mixer {

struct anderson_darling_stats {
	double A2{};
};

inline anderson_darling_stats anderson_darling(std::vector<double> data) {
	std::sort(data.begin(), data.end());
	double sum = 0;
	for (std::size_t i = 0; i < data.size(); ++i) {
		sum += (2. * i - 1.) *
			std::log(data[i] * (1. - data[data.size() - 1 - i]));
	}
	const auto n = static_cast<double>(data.size());
	return {-n - sum / n};
}

inline anderson_darling_stats anderson_darling_test(const uint64_t n, const stream& stream) {
	return anderson_darling(get_normalized(n, stream));
}

}
