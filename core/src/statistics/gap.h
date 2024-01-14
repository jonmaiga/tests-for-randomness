#pragma once

#include <cmath>
#include <vector>

#include "chi2.h"
#include "streams.h"
#include "types.h"
#include "util/algo.h"

namespace tfr {
template <typename T>
std::vector<uint64_t> generate_gaps(uint64_t max_gap_size, double a, double b, const T& data) {
	std::vector<uint64_t> gaps(max_gap_size);
	std::size_t current_gap = 0;
	if (is_near(b, 1., 1e-14)) {
		b = 2.;
	}
	for (const auto vv : data) {
		const auto v = rescale_type_to_01(vv);
		if (v >= a && v < b) {
			gaps[std::min(current_gap, gaps.size() - 1)]++;
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

template <typename T>
sub_test_results gap_test(uint64_t n, const stream<T>& source) {
	sub_test_results results;
	for (const int wanted_gaps : {2}) {
		const double gap_size = 1. / static_cast<double>(wanted_gaps);
		const uint64_t expected_total_count = n / wanted_gaps;
		for (int gi = 0; gi < wanted_gaps; ++gi) {
			const double a = snap_to_01(gi * gap_size);
			const double b = snap_to_01(a + gap_size);

			const auto& ps = generate_gap_probabilities(a, b);
			const auto& gaps = generate_gaps(ps.size(), a, b, ranged_stream(source, n));

			if (const auto s = chi2_stats(gaps.size(), to_data(gaps),
			                              mul(to_data(ps), to_data(expected_total_count)),
			                              5.)) {
				results.push_back({"g" + std::to_string(gi), s});
			}
		}
	}
	return results;
}
}
