#pragma once

#include <iostream>
#include <vector>

#include "chi2.h"
#include "types.h"

namespace tfr {

int berlekamp_massey_(const std::vector<int>& u) {
	const auto len = u.size();
	std::vector<int> b(len, 0);
	std::vector<int> c(len, 0);
	b[0] = c[0] = 1;

	int l = 0;
	int m = 0;

	for (int n = 1; n <= len; ++n) {
		int s = u[n-1];
		for (int j = 1; j <= l; ++j) {
			s ^= c[j] & u[n-j-1];  // instead of s += c[j] * u[n-j-1]; s toggles between 0 and 1
		}
		
		if (s != 0) {
			const int from = n - m;
			const int to = from + l;
			const std::vector<int> b_tmp(cbegin(b), cbegin(b) + l + 1);
			if (2 * l <= n) {
				l = n - l;
				m = n;
				b = c;
			}
			
			for (int j = from; j <= to; ++j) {
				c[j] ^= b_tmp[j - from]; // same as c[j] = (c[j] + bsub[j - from]) % 2;
			}
		}
	}
	return l;
}

template <typename RangeT>
void for_each_block(const RangeT& data, uint64_t block_size, const std::function<void(const std::vector<int>&)>& callback) {
	auto block = std::vector<int>(block_size, 0);
	for_each_bit(data, [i=0ull, block_size, &block, callback](int bit) mutable {
		block[i] = static_cast<int>(bit);
		if (++i == block_size) {
			callback(block);
			i = 0;
		}
	});
}

template <typename T>
std::optional<statistic> linear_complexity_stats(uint64_t n, stream<T> stream, uint64_t block_size) {
	auto ps = std::vector<double>();
	if (block_size % 2 == 0) {
    	ps = {1 / 96., 1 / 32., 1 / 8., 1 / 2., 1 / 4., 1 / 16., 1 / 48.};
	} else {
		ps = {1 / 48., 1 / 16., 1 / 4., 1 / 2., 1 / 8., 1 / 32., 1 / 96.};
	}

	auto median = (block_size + 1) / 2;
	std::vector<uint64_t> counts(7, 0);
	auto count = 0;
	for_each_block(ranged_stream<T>(stream, n), block_size,
	                [median, &counts, &count](const std::vector<int>& bits) {
		                const auto l = berlekamp_massey_(bits);
						if (l <= median - 3) ++counts[0];
						else if (l >= median + 3) ++counts[6];
						else ++counts[l - median + 3];
		                ++count;
	                });

	return chi2_stats(counts.size(), to_data(counts),
	                  mul(to_data(ps), to_data(count)), 5.);
}

template <typename T>
uint64_t get_block_size(uint64_t n) {
	constexpr double wanted_blocks = 5. / (1./96);
	constexpr double multiplier = bit_sizeof<T>() / wanted_blocks;
	return n * multiplier;
}

template <typename T>
sub_test_results linear_complexity_test(uint64_t n, const stream<T>& source) {
	const auto block_size = get_block_size<T>(n);
	return {{std::to_string(block_size), linear_complexity_stats(n, source, block_size)}};
}

}
