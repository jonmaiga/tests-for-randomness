#pragma once

#include <vector>

#include "streams.h"
#include "util/math.h"

namespace mixer {

inline double compute_chi2_test(const std::vector<uint64_t>& bins, double expected_count) {
	double chi2 = 0;
	for (const auto bin : bins) {
		const double diff = static_cast<double>(bin) - expected_count;
		chi2 += diff * diff / expected_count;
	}

	return chi2;
}

inline std::vector<statistic> chi2_test(const uint64_t n, const stream& stream) {
	std::vector<uint64_t> bins(static_cast<std::size_t>(1.88 * std::pow(n, 0.4)));
	const auto binCount = static_cast<double>(bins.size());
	for (uint64_t i = 0; i < n; ++i) {
		auto index = static_cast<std::size_t>(binCount * normalize(stream()));
		bins[std::min(bins.size() - 1, index)]++;
	}
	const double expected_count = static_cast<double>(n) / binCount;
	const double chi2 = compute_chi2_test(bins, expected_count);
	const auto df = static_cast<double>(bins.size() - 1);
	return {{s_type::chi2, chi2, gamma_regularized(df * .5, chi2 * 0.5)}};
}

}
