#pragma once

#include "combiner.h"
#include "mixers32.h"

namespace mixer {

using combiner32 = combiner<uint32_t>;

namespace combine32 {

// 10
const combiner32 xmx = {
	"c-xmx", [](uint32_t x, uint32_t y) {
		x += 2471660141;
		y -= 2471660141;

		y ^= (y >> 16);
		y *= 2471660141;
		x ^= y;
		x ^= (x >> 15);
		return x;
	}
};

// 14, weak to (counter-1, c) (10)
const combiner32 xm2x = {
	"c-xm2x", [](uint32_t x, uint32_t y) {
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

// 21
const combiner32 xm3x = {
	"c-xm3x", [](uint32_t x, uint32_t y) {
		
		x += 2471660141;
		y -= 2471660141;
		return mix32::xm2x(x ^ mix32::xmx(y));
		y ^= (y >> 16);
		y *= 2471660141;
		x ^= y;
		x ^= (x >> 15);
		x *= 2471660141;
		x ^= (x >> 16);
		x *= 2471660141;
		x ^= (x >> 15);
		return x;
	}
};

const combiner32 boost = {
	"c-boost", [](uint32_t x, uint32_t y) {
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


}}