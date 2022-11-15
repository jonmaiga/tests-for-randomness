#pragma once

#include <set>
#include <vector>

#include "chi2.h"
#include "distributions.h"
#include "types.h"
#include "util/algo.h"

namespace mixer {

inline std::vector<uint64_t> collect_divisible(uint64_t wanted, uint64_t tracked, const std::vector<uint64_t>& data) {
	std::set<uint64_t> coupons_collected;
	std::vector<uint64_t> draws_histogram(tracked + 1);
	uint64_t draw_count = 0;
	uint64_t collected = 0;
	for (const auto& v : data) {
		++draw_count;
		if (v % 2 != 0) {
			continue;
		}
		if (++collected < wanted) {
			continue;
		}
		std::size_t index = draw_count - wanted;
		index = std::min(draws_histogram.size() - 1, index);
		draws_histogram[index] ++;
		draw_count = 0;
		collected = 0;
	}
	return draws_histogram;
}

inline std::vector<double> divisible_expected_probabilities(const uint64_t wanted, const uint64_t tracked) {
	std::vector<double> expected;
	double sum = 0;
	for (uint64_t k = 0; k < tracked; ++k) {
		const auto p = negative_binomial_pdf(wanted, 1./2., k);
		expected.push_back(p);
		sum += p;
	}
	expected.push_back(1. - sum);
	return expected;
}

inline chi2_statistics divisibility_stats(const std::vector<uint64_t>& data) {
	constexpr uint64_t divisor = 3;
	constexpr uint64_t wanted = 7;
	constexpr uint64_t tracked = 20;
	const auto cc = collect_divisible(wanted, tracked, data);
	const auto ps = divisible_expected_probabilities(wanted, tracked);
	assertion(cc.size() == ps.size(), "Unexpected size in divisible");
	const auto total_count = accumulate(cc);
	return chi2_stats(
		cc.size(),
		to_data(cc),
		mul(to_data(ps), to_data(total_count)),
		1.);
}

inline std::vector<statistic> divisibility_test(uint64_t n, const stream& stream) {
	const auto stats = divisibility_stats(get_raw(n, stream));
	const auto p_value = chi2_distribution_cdf(stats.chi2, stats.df);
	assertion(is_valid_between_01(p_value), "bad p value");
	return {{s_type::divisibility, stats.chi2, p_value}};
}


}
