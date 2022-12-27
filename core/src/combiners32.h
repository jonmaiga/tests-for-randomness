#pragma once

#include "combiner.h"

namespace mixer {

using combiner32 = combiner<uint32_t>;

namespace combine32 {

const combiner32 xmx = {
	"c-xmx", [](uint32_t x, uint32_t y) {
		// x=mix32::xm3x(x);
		// y=mix32::xm3x(y);
		x ^= (x >> 16);
		x *= 2471660141;
		x ^= y;
		x ^= (x >> 15);
		return x ^ y;
	}
};


}}
