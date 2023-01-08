#pragma once

#include <algorithm>
#include <bitset>
#include <cctype>
#include <numeric>
#include <vector>

#include "bitwise.h"
#include "math.h"
#include "stream.h"
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


template <typename T>
std::vector<T> get_raw_by_ref(uint64_t n, stream<T>& stream) {
	std::vector<T> data;
	data.reserve(n);
	for (uint64_t i = 0; i < n; ++i) {
		data.push_back(stream());
	}
	return data;
}

template <typename T>
std::vector<double> rescale_type_to_01_by_ref(uint64_t n, stream<T>& stream) {
	std::vector<double> data;
	data.reserve(n);
	for (uint64_t i = 0; i < n; ++i) {
		data.push_back(rescale_type_to_01(stream()));
	}
	return data;
}

template <typename T>
std::vector<double> rescale_type_to_01(const std::vector<T>& data) {
	std::vector<double> ns;
	ns.reserve(data.size());
	for (const auto v : data) {
		ns.push_back(rescale_type_to_01(v));
	}
	return ns;
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
std::vector<double> rescale_to_01(const std::vector<T>& data) {
	return data.empty()
		       ? std::vector<double>{}
		       : rescale_to_01(data,
		                       *std::min_element(data.begin(), data.end()),
		                       *std::max_element(data.begin(), data.end()));
}

struct xys {
	std::vector<double> xs;
	std::vector<double> ys;
};

template <typename T>
xys create_bit_flipped_xy(uint64_t n, stream<T> source, const mixer<T>& mixer) {
	constexpr auto Bits = bit_sizeof<T>();
	std::vector<double> xs, ys;
	for (uint64_t i = 0; i < n; ++i) {
		const T v = source();
		const T m = mixer(v);
		const double x = rescale_type_to_01(m);
		for (int bit = 0; bit < Bits; ++bit) {
			xs.push_back(x);
			ys.push_back(rescale_type_to_01(mixer(flip_bit(m, bit))));
		}
	}
	return {xs, ys};
}

template <typename T>
xys create_serial_xy_by_ref(uint64_t n, stream<T>& source) {
	std::vector<double> xs, ys;
	for (uint64_t i = 0; i < n; ++i) {
		xs.push_back(rescale_type_to_01(source()));
		ys.push_back(rescale_type_to_01(source()));
	}
	return {xs, ys};
}

template <typename T>
T isolate_bit_by_ref(stream<T>& source, int bit) {
	constexpr auto Bits = bit_sizeof<T>();
	T x = 0;
	const T m = 1ull << bit;
	for (int i = 0; i < Bits; ++i) {
		const auto v = source();
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

template <typename T>
void sliding_bit_window(
	const T& data,
	int window_size,
	int increments,
	const std::function<void(uint64_t)>& callback) {
	constexpr auto Size = bit_sizeof<typename T::value_type>();
	assertion(window_size >= 1 && window_size <= Size-1, "bad window size");
	assertion(increments >= 1 && increments + window_size <= Size, "bad increments");

	std::bitset<2 * Size> left_mask;
	left_mask |= (1ull << window_size) - 1;

	auto it = data.begin();
	const auto end = data.end();
	std::bitset<2 * Size> bits;
	bits |= *it;
	int bits_left_to_consume = Size;
	while (it != end) {
		while (bits_left_to_consume >= window_size) {
			callback((bits & left_mask).to_ullong());
			bits >>= increments;
			bits_left_to_consume -= increments;
		}
		++ it;
		if (it != end) {
			std::bitset<2 * Size> refill;
			refill |= *it;
			refill <<= bits_left_to_consume;
			bits |= refill;
			bits_left_to_consume += Size;
		}
	}
}


}
