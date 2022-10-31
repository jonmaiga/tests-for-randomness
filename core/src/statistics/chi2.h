#pragma once

#include <vector>

#include "distributions.h"
#include "streams.h"
#include "util/math.h"

namespace mixer {

struct chi2_stats {
	double chi2{};
	uint64_t df{};
};

inline chi2_stats compute_chi2(const std::vector<double>& normalized_data) {
	const auto bin_count = static_cast<uint64_t>(std::ceil(2 * pow(normalized_data.size(), .4)));
	std::vector<uint64_t> bins(bin_count);
	const auto binCount = static_cast<double>(bins.size());
	for (const auto v : normalized_data) {
		auto index = static_cast<std::size_t>(binCount * v);
		bins[std::min(bins.size() - 1, index)]++;
	}

	const double expected_count = static_cast<double>(normalized_data.size()) / binCount;
	double chi2 = 0;
	for (const auto bin : bins) {
		const double diff = static_cast<double>(bin) - expected_count;
		chi2 += diff * diff / expected_count;
	}
	return {chi2, bins.size() - 1};
}


inline std::vector<statistic> chi2_test(const uint64_t n, const stream& stream) {
	std::vector<double> data;
	data.reserve(n);
	for (uint64_t i = 0; i < n; ++i) {
		data.push_back(normalize(stream()));
	}
	const auto stat = compute_chi2(data);
	return {{s_type::chi2, stat.chi2, chi2_distribution_cdf(stat.chi2, stat.df)}};
}

}