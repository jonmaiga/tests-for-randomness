#pragma once

#include <iostream>
#include <set>
#include <vector>

#include "chi2.h"
#include "distributions.h"
#include "types.h"
#include "util/algo.h"

namespace mixer {

inline std::vector<uint64_t> collect_divisible(uint64_t divisor, uint64_t wanted, uint64_t tracked, const std::vector<uint64_t>& data) {
	std::set<uint64_t> coupons_collected;
	std::vector<uint64_t> draws_histogram(tracked);
	uint64_t draw_count = 0;
	uint64_t collected = 0;
	for (const auto& v : data) {
		++draw_count;
		if (v % divisor != 0) {
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

inline std::vector<double> divisible_expected_probabilities(const uint64_t divisor, const uint64_t wanted) {
	std::vector<double> expected;
	double sum = 0;
	uint64_t k = 0;
	while (sum < 0.99) {
		const auto p = negative_binomial_pdf(wanted, 1. / divisor, k);
		expected.push_back(p);
		sum += p;
		++k;
	}
	expected.push_back(1. - sum);
	return expected;
}


inline std::vector<statistic> divisibility_test(uint64_t n, const stream& stream) {
	const auto& data = get_raw(n, stream);
	//constexpr uint64_t wanted = 7;

	std::vector<statistic> results;
	for (const uint64_t divisor : {2, 3, 5, 6, 7}) {
		const auto wanted = 5;
		const auto ps = divisible_expected_probabilities(divisor, wanted);
		const auto collected = collect_divisible(divisor, wanted, ps.size(), data);
		assertion(collected.size() == ps.size(), "Unexpected size in divisible");

		const auto total_count = accumulate(collected);
		const auto stats = chi2_stats(collected.size(), to_data(collected),
			mul(to_data(ps), to_data(total_count)), 1.);
		if (stats.df < 10.) {
			continue;
		}
		const auto p_value = chi2_distribution_cdf(stats.chi2, stats.df);
		assertion(is_valid_between_01(p_value), "bad p value");
		results.emplace_back(s_type::divisibility, stats.chi2, p_value);
	}
	return results;
}


}
