#pragma once

#include <string>
#include <vector>

#include "streams.h"
#include "types.h"

namespace mixer {

struct chi2_stats {
	double chi2{};
};

struct chi2_result {
	std::string stream_name;
	std::string mixer_name;
	uint64_t n;
	chi2_stats stats;
};

inline chi2_stats compute_chi2_test(const std::vector<uint64_t>& bins, double expected_count) {
	double chi2 = 0;
	for (const auto bin : bins) {
		const double diff = static_cast<double>(bin) - expected_count;
		chi2 += diff * diff / expected_count;
	}
	return {chi2};
}

inline chi2_result chi2_test(const uint64_t n, const stream& stream, const mixer& mixer) {
	const auto mixer_stream = create_stream_from_mixer(stream, mixer);
	constexpr auto normalizer = static_cast<double>(std::numeric_limits<uint64_t>::max());

	std::vector<uint64_t> bins(static_cast<std::size_t>(1.88 * std::pow(n, 0.4)));
	const auto binCount = static_cast<double>(bins.size());
	try {
		for (uint64_t i = 0; i < n; ++i) {
			const auto d = static_cast<double>(mixer_stream());
			bins[static_cast<std::size_t>(binCount * (d / normalizer))]++;
		}
	}
	catch (const std::runtime_error&) {
		throw;
	}
	const double expected_count = static_cast<double>(n) / binCount;
	return {stream.name, mixer.name, n, compute_chi2_test(bins, expected_count)};
}

}
