#pragma once

#include <algorithm>
#include <cctype>
#include <numeric>
#include <vector>

#include "bitwise.h"
#include "math.h"
#include "types.h"

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

inline std::vector<double> rescale64_to_01(uint64_t n, const stream& stream) {
	std::vector<double> data;
	data.reserve(n);
	for (uint64_t i = 0; i < n; ++i) {
		data.push_back(rescale64_to_01(stream()));
	}
	return data;
}

template <typename T>
std::vector<double> rescale_to_01(const std::vector<T>& data, T min_value, T max_value) {
	std::vector<double> ns;
	ns.reserve(data.size());
	for (const auto v : data) {
		ns.push_back(rescale_to_01(v, min_value, max_value));
	}
	return ns;
}

template <typename T>
std::vector<double> rescale64_to_01(const std::vector<T>& data) {
	std::vector<double> ns;
	ns.reserve(data.size());
	for (const auto v : data) {
		ns.push_back(rescale64_to_01(v));
	}
	return ns;
}

template <typename T>
std::vector<double> rescale_to_01(const std::vector<T>& data) {
	return rescale_to_01(data,
	                     *std::min_element(data.begin(), data.end()),
	                     *std::max_element(data.begin(), data.end()));
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
		const double x = rescale64_to_01(m);
		for (int bit = 0; bit < 64; ++bit) {
			xs.push_back(x);
			ys.push_back(rescale64_to_01(mixer(flip_bit(m, bit))));
		}
	}
	return {xs, ys};
}

inline xys create_serial_xy(uint64_t n, const stream& source) {
	std::vector<double> xs, ys;
	for (uint64_t i = 0; i < n; ++i) {
		xs.push_back(rescale64_to_01(source()));
		ys.push_back(rescale64_to_01(source()));
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

inline bool contains(const std::string& str, const std::string& toFind) {
	return str.find(toFind) != std::string::npos;
}

inline bool containsIgnoreCase(const std::string& str, const std::string& toFind) {
	auto it = std::search(str.begin(), str.end(), toFind.begin(), toFind.end(), [](char ch1, char ch2) {
		                      return std::tolower(ch1) == std::tolower(ch2);
	                      }
	);
	return it != str.end();
}

inline std::string join(const std::vector<std::string>& strs, const std::string& delimiter) {
	std::string r;
	for (const auto& s : strs) {
		if (s.empty()) continue;
		if (!r.empty()) r += delimiter;
		r += s;
	}
	return r;
}

template <typename T>
typename T::value_type accumulate(const T& data) {
	return std::accumulate(data.begin(), data.end(), 0ull, std::plus());
}

inline std::vector<uint64_t> sliding_bit_window(
	const std::vector<uint64_t>& data,
	int window_size,
	int increments = 1) {

	assertion(window_size >= 1 && window_size <= 63, "bad window size");
	assertion(increments >= 1 && increments + window_size <= 63, "bad increments");

	std::vector<uint64_t> r;
	const uint64_t mask = (1ull << window_size) - 1;

	for (std::size_t i = 0; i < data.size(); ++i) {

		const uint64_t v = data[i];

		for (int left_bit = 0; left_bit < 64; left_bit += increments) {
			const int right_bit = left_bit + window_size;
			if (right_bit < 64) {
				r.push_back((v >> left_bit) & mask);
				continue;
			}
			if (i + 1 == data.size()) {
				r.push_back((v >> left_bit) & mask);
				break;
			}

			const auto b1 = (v >> left_bit) & mask;
			const auto w = data[i + 1];
			const uint64_t rm = (1ull << right_bit) - 1;
			const auto b2 = (w & rm) << (64 - left_bit);
			const auto q = b1 | b2;
			r.push_back(q);
		}
	}

	return r;
}

}
