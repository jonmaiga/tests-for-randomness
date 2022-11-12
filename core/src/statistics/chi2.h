#pragma once

#include <vector>

#include "distributions.h"
#include "streams.h"
#include "util/algo.h"
#include "util/math.h"

namespace mixer {

inline std::vector<uint64_t> bin_data_for_chi2(const std::vector<double>& data01) {
	std::vector<uint64_t> bins(static_cast<uint64_t>(std::ceil(2 * pow(data01.size(), .4))));
	const auto bin_count = static_cast<double>(bins.size());
	for (const auto v : data01) {
		assertion(is_valid_between_01(v), "Invalid data in chi2");
		auto index = static_cast<std::size_t>(bin_count * v);
		bins[std::min(bins.size() - 1, index)]++;
	}
	return bins;
}

struct chi2_statistics {
	double chi2{};
	double df{};
};

inline chi2_statistics chi2_from_bins(const std::vector<uint64_t>& bins, double expected_count) {
	double chi2 = 0;
	for (const auto count : bins) {
		const double diff = static_cast<double>(count) - expected_count;
		chi2 += diff * diff / expected_count;
	}
	return {chi2, static_cast<double>(bins.size() - 1)};
}

inline chi2_statistics chi2_uniform_stats(const std::vector<double>& data01) {
	const auto& bins = bin_data_for_chi2(data01);
	const double expected_count = static_cast<double>(data01.size()) / static_cast<double>(bins.size());
	return chi2_from_bins(bins, expected_count);
}

inline std::vector<statistic> chi2_test(const uint64_t n, const stream& stream) {
	const auto stat = chi2_uniform_stats(rescale64_to_01(n, stream));
	return {{s_type::chi2, stat.chi2, chi2_distribution_cdf(stat.chi2, stat.df)}};
}

}
