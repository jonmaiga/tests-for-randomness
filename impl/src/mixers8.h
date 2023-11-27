#pragma once

#include <vector>
#include "mixer.h"

namespace tfr {
using mixer8 = mixer<uint8_t>;

namespace mix8 {
const mixer8 xm3x = {
	"mix8::xm3x", [](uint8_t x) {
		constexpr uint8_t C = 119;
		x ^= (x >> 4);
		x *= C;
		x ^= (x >> 3);
		x *= C;
		x ^= (x >> 4);
		x *= C;
		x ^= (x >> 3);
		return x;
	}
};
}


template <>
inline mixer<uint8_t> get_default_mixer() {
	return mix8::xm3x;
}

template <>
inline std::vector<mixer8> get_mixers() {
	return {
		mix8::xm3x,
	};
}
}
