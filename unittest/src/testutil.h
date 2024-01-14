#pragma once

#include "mixers32.h"
#include "mixers64.h"
#include "streams.h"
#include "trng_data.h"

namespace tfr {
template <typename T> mixer<T> get_test_mixer();

template <> inline mixer<uint8_t> get_test_mixer() {
	return {
		"mix8:mx3_64", [](uint8_t x) { return static_cast<uint8_t>(mix64::mx3(x)); }
	};
}

template <> inline mixer<uint16_t> get_test_mixer() {
	return {
		"mix16:mx3_64", [](uint16_t x) { return static_cast<uint16_t>(mix64::mx3(x)); }
	};
}

template <> inline mixer<uint32_t> get_test_mixer() {
	return {
		"mix32:mx3_64", [](uint32_t x) { return static_cast<uint32_t>(mix64::mx3(x)); }
	};
}

template <> inline mixer<uint64_t> get_test_mixer() { return mix64::mx3; }

template <typename T = uint64_t>
stream<T> test_stream() {
	return create_stream_from_mixer<T>(create_counter_stream<T>(1), get_test_mixer<T>());
}

template <typename T>
auto test_stream_casted(uint64_t n) {
	auto s = test_stream();
	std::vector<T> data;
	for (int i = 0; i < n; ++i) {
		data.push_back(static_cast<T>(s()));
	}
	return create_stream_from_data("test", data);
}

template <typename T>
uint64_t hash64(const T& data) {
	static_assert(sizeof(typename T::value_type) == sizeof(uint64_t));
	uint64_t h = 0;
	for (auto v : data) {
		h += mix64::mx3(*reinterpret_cast<uint64_t*>(&v));
	}
	return h;
}

inline std::vector<uint64_t> rescale_01_to_type(const std::vector<double>& d) {
	std::vector<uint64_t> data;
	for (const double p : d) {
		const auto max = std::numeric_limits<uint64_t>::max();
		data.push_back(static_cast<uint64_t>(p * max));
	}
	return data;
}
}
