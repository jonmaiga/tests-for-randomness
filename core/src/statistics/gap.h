#pragma once

#include <cmath>
#include <vector>

#include "chi2.h"
#include "distributions.h"
#include "types.h"
#include "util/algo.h"

namespace mixer {

inline std::vector<uint64_t> generate_gaps(uint64_t max_gap_size, double a, double b, const std::vector<double>& data01) {
	std::vector<uint64_t> gaps(max_gap_size);
	uint64_t current_gap = 0;
	for (const auto v : data01) {
		if (v >= a && v < b) {
			gaps[std::min(current_gap, gaps.size() - 1)] ++;
			current_gap = 0;
		}
		else {
			++current_gap;
		}
	}
	return gaps;
}

inline std::vector<double> generate_gap_probabilities(double a, double b) {
	std::vector<double> ps;
	double p_sum = 0;
	std::size_t i = 0;
	while (p_sum < 0.99) {
		ps.push_back((b - a) * std::pow(1. - b + a, i));
		p_sum += ps.back();
		++i;
	}
	ps.push_back(1. - p_sum);
	assertion(is_valid_between_01(ps.back()), "Weird gap probabilities");
	return ps;
}

inline std::vector<statistic> gap_test(uint64_t n, const stream& stream) {
	struct gap_test {
		test_type type;
		double a{};
		double b{};
	};

	static std::vector<gap_test> sub_tests = {
		{test_type::gap_low, 0, 0.33},
		{test_type::gap_medium, 0.33, 0.66},
		{test_type::gap_high, 0.66, 1},
	};

	const auto& data01 = rescale64_to_01(n, stream);
	std::vector<statistic> results;
	for (const auto& test : sub_tests) {
		const auto& ps = generate_gap_probabilities(test.a, test.b);
		const auto& gaps = generate_gaps(ps.size(), test.a, test.b, data01);
		const auto total_count = accumulate(gaps);
		const auto& stats = chi2_stats(gaps.size(), to_data(gaps),
		                               mul(to_data(ps), to_data(total_count)),
		                               1.);

		const auto p_value = chi2_distribution_cdf(stats.chi2, stats.df);
		assertion(is_valid_between_01(p_value), "bad p value");
		results.push_back({test.type, stats.chi2, p_value});
	}
	return results;
}

}
