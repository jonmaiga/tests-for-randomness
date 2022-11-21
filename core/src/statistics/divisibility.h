#pragma once

#include <iostream>
#include <set>
#include <vector>

#include "chi2.h"
#include "distributions.h"
#include "types.h"
#include "util/algo.h"

namespace mixer {

template <typename T>
std::vector<uint64_t> collect_divisible(uint64_t divisor, uint64_t wanted, uint64_t tracked, const T& data) {
	static_assert(std::is_integral_v<typename T::value_type>);
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

template <typename T>
std::optional<statistic> divisibility_test(uint64_t n, const stream<T>& stream, int divisor) {
	constexpr auto wanted = 7;
	const auto ps = divisible_expected_probabilities(divisor, wanted);
	const auto collected = collect_divisible(divisor, wanted, ps.size(), ranged_stream(stream, n));
	assertion(collected.size() == ps.size(), "Unexpected size in divisible");

	const auto total_count = accumulate(collected);
	if (const auto stats = chi2_stats(collected.size(), to_data(collected),
	                                  mul(to_data(ps), to_data(total_count)), 1.)) {
		if (stats->df < 5.) {
			return {};
		}
		return stats;
	}
	return {};
}

template <typename T>
stream_test<T> create_divisibility_test(int divisor) {
	return [divisor](uint64_t n, const stream<T>& stream) {
		return divisibility_test<T>(n, stream, divisor);
	};
}

}
