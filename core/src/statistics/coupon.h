#pragma once

#include <set>
#include <vector>

#include "distributions.h"
#include "types.h"
#include "util/algo.h"

namespace mixer {

inline std::vector<uint64_t> collect_coupons(uint64_t wanted_coupons, uint64_t tracked_draws, const std::vector<double>& data01) {
	std::set<uint64_t> coupons_collected;
	std::vector<uint64_t> buckets(tracked_draws + 1);
	uint64_t draw_count = 0;
	for (const auto& v : data01) {
		auto coupon_id = std::min(static_cast<uint64_t>(wanted_coupons * v), wanted_coupons - 1);
		++draw_count;
		if (!coupons_collected.insert(coupon_id).second) {
			continue;
		}

		if (coupons_collected.size() == wanted_coupons) {
			auto index = draw_count - wanted_coupons;
			index = std::min(buckets.size() - 1, index);
			buckets[index] ++;
			draw_count = 0;
			coupons_collected = {};
		}
	}
	return buckets;
}

inline std::vector<double> expected_probabilities(const uint64_t d, const uint64_t t) {
	// from: https://www.cs.fsu.edu/~mascagni/Testing.pdf
	std::vector<double> expected;
	const double fac_d = std::tgamma(d + 1);
	double sum = 0;
	for (auto i = d; i < d + t; ++i) {
		const double p = fac_d / pow(d, i) * stirling_second_kind(i - 1, d - 1);
		expected.push_back(p);
		sum += p;
	}
	expected.push_back(1. - sum);
	return expected;
}

struct coupon_statistics {
	double chi2{};
	double df{};
};

inline coupon_statistics coupon_stats(const std::vector<double>& data01) {
	constexpr uint64_t wanted_coupons = 5;
	constexpr uint64_t tracked_draws = 20;
	const auto cc = collect_coupons(wanted_coupons, tracked_draws, data01);
	const auto ps = expected_probabilities(wanted_coupons, tracked_draws);
	assertion(cc.size() == ps.size(), "Unexpected size in coupons");

	const auto total_count = std::accumulate(cc.begin(), cc.end(), 0ull, std::plus());
	double chi2 = 0;
	double df = 0;
	for (std::size_t i = 0; i < cc.size(); ++i) {
		const double expected_draws = ps[i] * total_count;
		const double diff = static_cast<double>(cc[i]) - expected_draws;
		chi2 += diff * diff / expected_draws;
		df += 1;
	}
	return {chi2, std::max(df - 1, 0.)};
}

inline std::vector<statistic> coupon_test(uint64_t n, const stream& stream) {
	const auto stats = coupon_stats(rescale64_to_01(n, stream));
	const auto p_value = chi2_distribution_cdf(stats.chi2, stats.df);
	assertion(is_valid_between_01(p_value), "bad p value");
	return {{s_type::coupon, stats.chi2, p_value}};
}


}
