#pragma once

#include <vector>

#include "distributions.h"

namespace mixer {

inline double fishers_combined_probabilities(const std::vector<double>& p_values) {
	constexpr double epsilon = 1e-9;
	double sum = 0;
	for (auto p : p_values) {
		p = std::max(epsilon, std::min(p, 1 - epsilon));
		sum += std::log(p);
	}
	return chi2_distribution_cdf(-2. * sum, 2. * p_values.size());
}


}
