#pragma once

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
sub_test_results divisibility_test(uint64_t n, const stream<T>& stream) {
	constexpr auto wanted = 5;

	sub_test_results results;
	for (uint64_t divisor = 2; divisor <= 3; ++divisor) {
		const auto ps = divisible_expected_probabilities(divisor, wanted);
		const auto collected = collect_divisible(divisor, wanted, ps.size(), ranged_stream(stream, n));
		assertion(collected.size() == ps.size(), "Unexpected size in divisible");

		const auto expected_total_count = n / (divisor * 5);
		if (const auto stats = chi2_stats(collected.size(), to_data(collected),
		                                  mul(to_data(ps), to_data(expected_total_count)), 5.)) {
			results.push_back({"d" + std::to_string(divisor), stats});
		}
	}
	return results;
}

}
