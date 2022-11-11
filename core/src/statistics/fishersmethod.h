#pragma once

#include <vector>

#include "distributions.h"
#include "util/math.h"

namespace mixer {

inline double fishers_combined_probabilities(const std::vector<double>& p_values) {
	double sum = 0;
	for (const auto p : p_values) {
		constexpr double min_p = 1e-16;
		sum += std::log(std::max(min_p, p));
	}
	assertion(is_valid(sum), "fisher sum is not valid");
	return chi2_distribution_cdf(-2. * sum, 2. * p_values.size());
}


}
