#pragma once

#include <algorithm>
#include <bitset>
#include <cctype>
#include <numeric>
#include <vector>

#include "bitwise.h"
#include "stream.h"
#include "types.h"

namespace tfr {

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

inline bool contains_ignore_case(const std::string& str, const std::string& toFind) {
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
void for_each_bit(const T& data, const std::function<void(bool)>& callback) {
	constexpr auto Size = bit_sizeof<typename T::value_type>();
	for (const auto v : data) {
		for (int b = 0; b < Size; ++b) {
			callback(is_bit_set(v, b));
		}
	}
}

template <typename RangeT>
void for_each_bit_block(const RangeT& data, uint64_t block_size, const std::function<void(const std::vector<int>&)>& callback) {
	auto block = std::vector<int>(block_size, 0);
	for_each_bit(data, [i=0ull, &block, callback](int bit) mutable {
		block[i] = bit;
		if (++i == block.size()) {
			callback(block);
			i = 0;
		}
	});
}

template <typename T>
void sliding_bit_window(const T& data, int window_size, const std::function<void(uint64_t)>& callback) {
	assertion(window_size > 0 && window_size < 64, "Invalid window size");
	uint64_t v = 0;
	int c = 0;

	const auto acc = [callback, c, v, window_size](bool is_set) mutable {
		v |= (is_set ? 1ull : 0ull) << c;
		++c;
		if (c == window_size) {
			callback(v);
			c = 0;
			v = 0;
		}
	};

	for_each_bit(data, acc);
}

struct merge_bins_result {
	std::vector<uint64_t> observed;
	std::vector<double> expected;
};

inline merge_bins_result merge_bins(
	const std::vector<uint64_t>& observed_counts,
	const std::vector<double>& expected_counts,
	double expected_threshold) {
	assertion(observed_counts.size() == expected_counts.size(), "observed size should equal expected size");

	merge_bins_result result{};
	uint64_t observed_sum = 0;
	double expected_sum = 0;
	for (std::size_t i = 0; i < observed_counts.size(); ++i) {
		expected_sum += expected_counts[i];
		observed_sum += observed_counts[i];
		if (expected_sum >= expected_threshold) {
			result.expected.push_back(expected_sum);
			result.observed.push_back(observed_sum);
			expected_sum = 0;
			observed_sum = 0;
		}
	}
	if (result.expected.empty()) {
		return result;
	}
	result.expected.back() += expected_sum;
	result.observed.back() += observed_sum;
	return result;
}

inline std::string replace_all(std::string str, const std::string& find, const std::string& replace) {
	if (find.empty()) {
		return str;
	}
	size_t pos = str.find(find);
	while (pos != std::string::npos) {
		str.replace(pos, find.size(), replace);
		pos = str.find(find, pos + replace.size());
	}
	return str;
}

inline std::string escape_for_md(std::string text) {
	static const std::vector<std::string> special_chars = {"\\", "`", "*", "_", "{", "}", "[", "]", "(", ")", "#", "+", "-", ".", "!"};
	for (const auto& c : special_chars) {
		text = replace_all(text, c, "\\" + c);
	}
	return text;
}

}
