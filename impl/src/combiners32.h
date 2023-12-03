#pragma once

#include "combiner.h"
#include "mixers32.h"

namespace tfr {
using combiner32 = combiner<uint32_t>;

namespace combine32 {
const combiner32 xmx = {
	"combine32::xmx", [](uint32_t x, uint32_t y) {
		x += 2471660141;
		y -= 2471660141;

		y ^= (y >> 16);
		y *= 2471660141;
		x ^= y;
		x ^= (x >> 15);
		return x;
	}
};

const combiner32 xm2x = {
	"combine32::xm2x", [](uint32_t x, uint32_t y) {
		x += 2471660141;
		y -= 2471660141;

		y ^= (y >> 16);
		y *= 2471660141;
		x ^= y;
		x ^= (x >> 15);
		x *= 2471660141;
		x ^= (x >> 14);
		return x;
	}
};

const combiner32 xm3x = {
	"combine32::xm3x", [](uint32_t x, uint32_t y) {
		x += 2471660141;
		y -= 2471660141;
		return mix32::mx2(x ^ mix32::mx1(y));
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
		return x;
	}
};
}


template <>
inline std::vector<combiner32> get_combiners() {
	return {
		combine32::xmx,
		combine32::xm2x,
		combine32::xm3x,
		combine32::boost
	};
}
}
