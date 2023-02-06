#pragma once

#include "distributions.h"
#include "util/math.h"

namespace tfr {

template <typename T>
double fishers_combined_probabilities(const T& p_values) {
	static_assert(std::is_floating_point_v<typename T::value_type>);
	double sum = 0;
	for (const auto p : p_values) {
		sum += log_safe(p);
	}
	assertion(is_valid(sum), "fisher sum is not valid");
	return chi2_distribution_cdf(-2. * sum, 2. * p_values.size());
}


}
