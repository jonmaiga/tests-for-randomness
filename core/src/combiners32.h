#pragma once

#include "combiner.h"

namespace mixer {

using combiner32 = combiner<uint32_t>;

namespace combine32 {

const combiner32 xmx = {
	"c-xmx", [](uint32_t x, uint32_t y) {
		y ^= (y >> 16);
		y *= 2471660141;
		y ^= x;
		y ^= (y >> 15);
		return y;
	}
};

const combiner32 xm2x = {
	"c-xm2x", [](uint32_t x, uint32_t y) {
		x += 2471660141;
		y ^= (y >> 16);
		y *= 2471660141;
		y ^= x;
		y ^= (y >> 15);
		y *= 2471660141;
		y ^= (y >> 14);
		return y;
	}
};


// template<class T>
// void hash_combine(size_t & seed, T const & v)
// {
//     seed ^= hash_value(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
// }


const auto boost_hash = [](uint32_t x) {
	x ^= x >> 16;
	x *= 0x21f0aaad;
	x ^= x >> 15;
	x *= 0x735a2d97;
	x ^= x >> 15;
	return x;
};

const combiner32 boost = {
	"c-boost", [](uint32_t x, uint32_t y) {
		x ^= boost_hash(y) + 0x9e3779b9 + (x << 6) + (x >> 2);
		return x;
	}
};


}}
