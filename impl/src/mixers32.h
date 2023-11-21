#pragma once

#include <vector>
#include "mixer.h"

namespace tfr {

using mixer32 = mixer<uint32_t>;

namespace mix32 {

const mixer32 xm3x = {
	"mix32::xm3x", [](uint32_t x) {
		constexpr uint32_t C = 2471660141;
		x ^= (x >> 16);
		x *= C;
		x ^= (x >> 15);
		x *= C;
		x ^= (x >> 16);
		x *= C;
		x ^= (x >> 15);
		return x;
	}
};

const mixer32 xm2x = {
	"mix32::xm2x", [](uint32_t x) {
		constexpr uint32_t C = 1939508389;
		x ^= x >> 16;
		x *= C;
		x ^= x >> 15;
		x *= C;
		x ^= x >> 14;
		return x;
	}
};

const mixer32 prospector = {
	"mix32::prospector", [](uint32_t x) {
		x ^= x >> 15;
		x *= 0x2c1b3c6dU;
		x ^= x >> 12;
		x *= 0x297a2d39U;
		x ^= x >> 15;
		return x;
	}
};

const mixer32 prospector_boost = {
	"mix32::prospector_boost", [](uint32_t x) {
		x ^= x >> 16;
		x *= 0x21f0aaad;
		x ^= x >> 15;
		x *= 0x735a2d97;
		x ^= x >> 15;
		return x;
	}
};

const mixer32 h2_sql = {
	"mix32::h2sql", [](uint32_t x) {
		// Thomas Mueller
		x = ((x >> 16) ^ x) * 0x45d9f3bU;
		x = ((x >> 16) ^ x) * 0x45d9f3bU;
		x = (x >> 16) ^ x;
		return x;
	}
};

const mixer32 murmur = {
	"mix32::murmur", [](uint32_t x) {
		x ^= x >> 16;
		x *= 0x85ebca6bU;
		x ^= x >> 13;
		x *= 0xc2b2ae35U;
		x ^= x >> 16;
		return x;
	}
};

const mixer32 xmx = {
	"mix32::xmx", [](uint32_t x) {
		constexpr uint32_t C = 2471660141;
		x ^= x >> 15;
		x *= C;
		x ^= x >> 16;
		return x;
	}
};

const mixer32 wang_1 = {
	"mix32::wang_1", [](uint32_t x) {
		x = (x ^ 61) ^ (x >> 16);
		x *= 9;
		x ^= x >> 4;
		x *= 0x27d4eb2d;
		x ^= x >> 15;
		return x;
	}
};

const mixer32 jenkins = {
	"mix32::jenkins", [](uint32_t x) {
		x = (x + 0x7ed55d16) + (x << 12);
		x = (x ^ 0xc761c23c) ^ (x >> 19);
		x = (x + 0x165667b1) + (x << 5);
		x = (x + 0xd3a2646c) ^ (x << 9);
		x = (x + 0xfd7046c5) + (x << 3);
		x = (x ^ 0xb55a4f09) ^ (x >> 16);
		return x;
	}
};

// uses 64-bits
const mixer32 splitmix64 = {
	"mix32::splitmix64", [](uint32_t x) {
		uint64_t z = x;
		z ^= z >> 33;
		z *= 0x62a9d9ed799705f5ull;
		z ^= z >> 28;
		z *= 0xcb24d0a5c88c35b3ull;
		return static_cast<uint32_t>(z >> 32);
	}
};

}


template <>
inline mixer<uint32_t> get_default_mixer() {
	return mix32::xm3x;
}

template <>
inline std::vector<mixer32> get_mixers() {
	return {
		mix32::xm3x,
		mix32::xm2x,
		mix32::prospector,
		mix32::prospector_boost,
		mix32::h2_sql,
		mix32::murmur,
		mix32::xmx,
		mix32::wang_1,
		mix32::jenkins
	};
}

}
