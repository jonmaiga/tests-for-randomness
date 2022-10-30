#pragma once

#include <numeric>
#include <vector>

#include "bitwise.h"
#include "math.h"

namespace mixer {

template <typename Container, typename ContainerRhs>
void append(Container& destination, const ContainerRhs& source) {
	destination.insert(destination.end(), source.begin(), source.end());
}

template <typename T, typename Compare>
std::vector<std::size_t> get_ranks(const std::vector<T>& vec, Compare& compare) {
	std::vector<std::size_t> p(vec.size());
	std::iota(p.begin(), p.end(), 0);
	std::sort(p.begin(), p.end(),
	          [&](std::size_t i, std::size_t j) { return compare(vec[i], vec[j]); });
	return p;
}


inline std::vector<uint64_t> get_raw(uint64_t n, const stream& stream) {
	std::vector<uint64_t> data;
	data.reserve(n);
	for (uint64_t i = 0; i < n; ++i) {
		data.push_back(stream());
	}
	return data;
}

inline std::vector<double> get_normalized(uint64_t n, const stream& stream) {
	std::vector<double> data;
	data.reserve(n);
	for (uint64_t i = 0; i < n; ++i) {
		data.push_back(normalize(stream()));
	}
	return data;
}

struct xys {
	std::vector<double> xs;
	std::vector<double> ys;
};

inline xys create_bit_flipped_xy(uint64_t n, const stream& source, const mixer& mixer) {
	std::vector<double> xs, ys;
	for (uint64_t i = 0; i < n; ++i) {
		const uint64_t v = source();
		const uint64_t m = mixer(v);
		const double x = normalize(m);
		for (int bit = 0; bit < 64; ++bit) {
			xs.push_back(x);
			ys.push_back(normalize(mixer(flip_bit(m, bit))));
		}
	}
	return {xs, ys};
}

inline uint64_t create_from_bit(const stream& source, int bit) {
	uint64_t x = 0;
	const uint64_t m = 1ull << bit;
	for (int i = 0; i < 64; ++i) {
		const auto v = source.next();
		if (v & m) {
			x |= 1ull << i;
		}
	}
	return x;
}

}
