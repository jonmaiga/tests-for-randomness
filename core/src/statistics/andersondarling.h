#pragma once

#include <vector>

#include "streams.h"
#include "types.h"
#include "util/algo.h"

namespace mixer {

struct anderson_darling_stats {
	double A2{};
	double p_value{};
};

inline double p_value(const double A2) {
	// values from: A Modified Anderson-Darling Test for Uniformity
	// exponential fit in mathematica, decays to quickly after A2 > 4
	// const auto vs = {{1.2453, 0.250}, {1.6211, 0.150}, {1.9355, 0.100}, {2.4986, 0.050}, {3.0916, 0.025}, {3.9033, 0.010}, {4.5416, 0.005}, {6.0266, 0.001}};
	const auto p_approximate = 1.2523881776721628 * std::exp(-1.2989101557411682 * A2);
	return std::min(std::max(0., p_approximate), 1.);

}

inline anderson_darling_stats anderson_darling(std::vector<double> data) {
	std::sort(data.begin(), data.end());
	double sum = 0;
	for (std::size_t i = 0; i < data.size(); ++i) {
		sum += (2. * i + 1.) * (
			std::log(data[i]) +
			std::log(1. - data[data.size() - 1 - i]));
	}

	const auto n = static_cast<double>(data.size());
	sum /= n;

	const double A2 = -n - sum;
	return {A2, p_value(A2)};
}

inline anderson_darling_stats anderson_darling_test(const uint64_t n, const stream& stream) {
	return anderson_darling(get_normalized(n, stream));
}

}
