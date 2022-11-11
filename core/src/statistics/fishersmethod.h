#pragma once

#include <vector>

#include "distributions.h"
#include "util/math.h"

namespace mixer {

inline double fishers_combined_probabilities(const std::vector<double>& p_values) {
	double sum = 0;
	for (auto p : p_values) {
		p = rescale_to_01(p, 0, 1);
		sum += std::log(p);
	}
	assertion(is_valid(sum), "fisher sum is not valid");
	return chi2_distribution_cdf(-2. * sum, 2. * p_values.size());
}


}
