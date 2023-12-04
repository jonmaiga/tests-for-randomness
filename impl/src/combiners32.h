#pragma once

#include "combiner.h"
#include "mixers32.h"

namespace tfr {
using combiner32 = combiner<uint32_t>;

namespace combine32 {
const combiner32 mx1 = {
	"combine32::mx1", [](uint32_t x, uint32_t y) {
		constexpr uint32_t C = 2471660141U;
		y ^= y >> 16;
		y *= C;
		x ^= y;
		x += C;
		x ^= x >> 14;
		x *= C;
		x ^= x >> 13;
		return x;
	}
};

const combiner32 mx2 = {
	"combine32::mx2", [](uint32_t x, uint32_t y) {
		constexpr uint32_t C = 1159349557U;
		y ^= y >> 16;
		y *= C;
		x ^= y;
		x += C;
		x ^= x >> 14;
		x *= C;
		x ^= x >> 13;
		x *= C;
		x ^= x >> 16;
		return x;
	}
};

const combiner32 mx3 = {
	"combine32::mx3", [](uint32_t x, uint32_t y) {
		constexpr uint32_t C = 1159349557U;
		y ^= y >> 16;
		y *= C;
		x ^= y;
		x += C;
		x ^= x >> 14;
		x *= C;
		x ^= x >> 13;
		x *= C;
		x ^= x >> 16;
		x *= C;
		x ^= x >> 14;
		return x;
	}
};

const combiner32 boost = {
	"combine32::boost", [](uint32_t x, uint32_t y) {
		static const auto boost_hash = [](uint32_t x) {
			x ^= x >> 16;
			x *= 0x21f0aaad;
			x ^= x >> 15;
			x *= 0x735a2d97;
			x ^= x >> 15;
			return x;
		};

		x ^= boost_hash(y) + 0x9e3779b9 + (x << 6) + (x >> 2);
		return x; // 10/15
	}
};
}


template <>
inline std::vector<combiner32> get_combiners() {
	return {
		combine32::mx1,
		combine32::mx2,
		combine32::mx3,
		combine32::boost
	};
}
}
