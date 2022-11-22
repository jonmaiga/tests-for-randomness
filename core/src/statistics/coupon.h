#pragma once

#include <set>
#include <vector>

#include "chi2.h"
#include "types.h"
#include "streams.h"
#include "util/algo.h"

namespace mixer {

template <typename T>
std::vector<uint64_t> collect_coupons(uint64_t wanted_coupons, uint64_t tracked_draws, const T& data01) {
	static_assert(std::is_floating_point_v<typename T::value_type>);
	std::set<uint64_t> coupons_collected;
	std::vector<uint64_t> draws_histogram(tracked_draws);
	uint64_t draw_count = 0;
	for (const auto& v : data01) {
		auto coupon_id = std::min(static_cast<uint64_t>(wanted_coupons * v), wanted_coupons - 1);
		++draw_count;
		if (!coupons_collected.insert(coupon_id).second) {
			continue;
		}

		if (coupons_collected.size() == wanted_coupons) {
			std::size_t index = draw_count - wanted_coupons;
			index = std::min(draws_histogram.size() - 1, index);
			draws_histogram[index] ++;
			draw_count = 0;
			coupons_collected = {};
		}
	}
	return draws_histogram;
}

inline std::vector<double> expected_probabilities(const uint64_t wanted_coupons) {
	// from: https://www.cs.fsu.edu/~mascagni/Testing.pdf
	std::vector<double> expected;
	const double fac_d = std::tgamma(wanted_coupons + 1);
	double sum = 0;
	auto i = wanted_coupons;
	while (sum < 0.99 && i < 30) {
		const double p = fac_d / pow(wanted_coupons, i) * stirling_second_kind(i - 1, wanted_coupons - 1);
		assertion(is_valid_between_01(p), "unexpected coupon probability");
		expected.push_back(p);
		sum += p;
		++i;
	}
	expected.push_back(1. - sum);
	return expected;
}

template <typename T>
std::optional<statistic> coupon_stats(const T& data01) {
	constexpr uint64_t wanted_coupons = 5;
	const auto ps = expected_probabilities(wanted_coupons);
	const auto cc = collect_coupons(wanted_coupons, ps.size(), data01);
	assertion(cc.size() == ps.size(), "Unexpected size in coupons");

	const auto total_count = accumulate(cc);
	return chi2_stats(cc.size(), to_data(cc), mul(to_data(ps), to_data(total_count)), 1.);
}

template <typename T>
sub_tests coupon_test(uint64_t n, const stream<T>& stream) {
	return main_sub_test(coupon_stats(ranged_stream(rescale_type_to_01(stream), n)));
}


}
