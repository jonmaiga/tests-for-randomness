#pragma once

#include <cmath>
#include <numeric>
#include <vector>

#include "distributions.h"
#include "types.h"
#include "util/algo.h"

namespace mixer {

inline std::vector<uint64_t> generate_gaps(uint64_t max_gap_size, double a, double b, const std::vector<double>& data01) {
	std::vector<uint64_t> gaps(max_gap_size + 1);

	uint64_t current_gap = 0;
	for (const auto v : data01) {
		if (v >= a && v < b) {
			gaps[std::min(current_gap, max_gap_size)] ++;
			current_gap = 0;
		}
		else {
			++current_gap;
		}
	}
	return gaps;
}

inline std::vector<double> generate_gap_probabilities(uint64_t max_gap_size, double a, double b) {
	std::vector<double> ps(max_gap_size + 1);
	double p_sum = 0;
	for (std::size_t i = 0; i < max_gap_size; ++i) {
		ps[i] = (b - a) * std::pow(1. - b + a, i);
		p_sum += ps[i];
	}
	ps[max_gap_size] = 1. - p_sum;
	assertion(is_valid_between_01(ps.back()), "Weird gap probabilities");
	return ps;
}


struct gap_statistics {
	double chi2{};
	double df{};
};

inline gap_statistics gap_stats(const std::vector<double>& data01) {
	constexpr uint64_t max_gap_size = 7;
	double a = 0.35;
	double b = 0.65;
	const auto& ps = generate_gap_probabilities(max_gap_size, a, b);
	const auto& gaps = generate_gaps(max_gap_size, a, b, data01);
	const auto total_count = std::accumulate(gaps.begin(), gaps.end(), 0.);
	double chi2 = 0;
	double df = 0;
	for (std::size_t i = 0; i < gaps.size(); ++i) {
		const double expected_count = ps[i] * total_count;
		if (expected_count < 5) continue;
		const double diff = static_cast<double>(gaps[i]) - expected_count;
		chi2 += diff * diff / expected_count;
		df += 1;
	}

	return {chi2, std::max(df - 1, 0.)};
}

inline std::vector<statistic> gap_test(uint64_t n, const stream& stream) {
	const auto stats = gap_stats(rescale64_to_01(n, stream));
	const auto p_value = chi2_distribution_cdf(stats.chi2, stats.df);
	assertion(is_valid_between_01(p_value), "bad p value");
	return {{s_type::gap, stats.chi2, p_value}};
}

}
