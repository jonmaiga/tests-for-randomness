#pragma once

#include <vector>
#include "mixer.h"

namespace tfr {
using mixer16 = mixer<uint16_t>;

namespace mix16 {
const mixer16 xm3x = {
	"mix16::xm3x", [](uint16_t x) {
		constexpr uint16_t C = 13487;
		x ^= (x >> 8);
		x *= C;
		x ^= (x >> 7);
		x *= C;
		x ^= (x >> 8);
		x *= C;
		x ^= (x >> 7);
		return x;
	}
};
}


template <>
inline mixer16 get_default_mixer() {
	return mix16::xm3x;
}

template <>
inline std::vector<mixer16> get_mixers() {
	return {
		mix16::xm3x,
	};
}
}
