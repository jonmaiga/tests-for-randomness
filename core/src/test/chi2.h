#pragma once

#include <vector>

#include "streams.h"
#include "util/math.h"

namespace mixer {

struct chi2_stats {
	double chi2{};
};

inline chi2_stats compute_chi2_test(const std::vector<uint64_t>& bins, double expected_count) {
	double chi2 = 0;
	for (const auto bin : bins) {
		const double diff = static_cast<double>(bin) - expected_count;
		chi2 += diff * diff / expected_count;
	}
	return {chi2};
}

inline chi2_stats chi2_test(const uint64_t n, const stream& stream, const mixer& mixer) {
	const auto mixer_stream = create_stream_from_mixer(stream, mixer);

	std::vector<uint64_t> bins(static_cast<std::size_t>(1.88 * std::pow(n, 0.4)));
	const auto binCount = static_cast<double>(bins.size());
	for (uint64_t i = 0; i < n; ++i) {
		bins[static_cast<std::size_t>(binCount * normalize(mixer_stream()))]++;
	}
	const double expected_count = static_cast<double>(n) / binCount;
	return compute_chi2_test(bins, expected_count);
}

}
