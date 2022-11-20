#pragma once

#include <cmath>
#include <vector>

#include "chi2.h"
#include "distributions.h"
#include "streams.h"
#include "types.h"
#include "util/algo.h"

namespace mixer {

template <typename T>
std::vector<uint64_t> generate_gaps(uint64_t max_gap_size, double a, double b, const T& data01) {
	static_assert(std::is_floating_point_v<typename T::value_type>);
	std::vector<uint64_t> gaps(max_gap_size);
	std::size_t current_gap = 0;
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

inline std::optional<statistic> gap_test(uint64_t n, const stream_uint64& source, test_type test, double a, double b) {
	const auto& ps = generate_gap_probabilities(a, b);
	const auto& gaps = generate_gaps(ps.size(), a, b, ranged_stream(rescale64_to_01(source), n));
	const auto total_count = accumulate(gaps);
	const auto& stats = chi2_stats(gaps.size(), to_data(gaps),
	                               mul(to_data(ps), to_data(total_count)),
	                               1.);

	const auto p_value = chi2_distribution_cdf(stats.chi2, stats.df);
	assertion(is_valid_between_01(p_value), "bad p value");
	return statistic{test, stats.chi2, p_value};
}

inline stream_test create_gap_test(test_type test, double a, double b) {
	return [test, a, b](uint64_t n, const stream_uint64& stream) {
		return gap_test(n, stream, test, a, b);
	};
}

}