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


inline chi2_statistics gap_stats(const std::vector<double>& data01) {
	double a = 0.35;
	double b = 0.65;
	const auto& ps = generate_gap_probabilities(a, b);
	const auto& gaps = generate_gaps(ps.size(), a, b, data01);
	const auto total_count = accumulate(gaps);
	return chi2_stats(gaps.size(), to_data(gaps),
	                  mul(to_data(ps), to_data(total_count)),
	                  5.);
}

inline std::vector<statistic> gap_test(uint64_t n, const stream& stream) {
	const auto stats = gap_stats(rescale64_to_01(n, stream));
	const auto p_value = chi2_distribution_cdf(stats.chi2, stats.df);
	assertion(is_valid_between_01(p_value), "bad p value");
	return {{s_type::gap, stats.chi2, p_value}};
}

}
