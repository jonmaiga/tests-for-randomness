#pragma once

#include <vector>
#include "mixer.h"

namespace tfr {
using mixer32 = mixer<uint32_t>;

namespace mix32 {
const mixer32 mx1 = {
	"mix32::mx1", [](uint32_t x) {
		constexpr uint32_t C = 2471660141U;
		x ^= x >> 16;
		x *= C;
		x ^= x >> 14;
		return x;
	}
};

const mixer32 mx2 = {
	"mix32::mx2", [](uint32_t x) {
		constexpr uint32_t C = 1159349557U;
		x ^= x >> 16;
		x *= C;
		x ^= x >> 14;
		x *= C;
		x ^= x >> 13;
		return x;
	}
};

const mixer32 mx3 = {
	"mix32::mx3", [](uint32_t x) {
		constexpr uint32_t C = 1159349557U;
		x ^= x >> 16;
		x *= C;
		x ^= x >> 14;
		x *= C;
		x ^= x >> 16;
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
		x *= 0x21f0aaadU;
		x ^= x >> 15;
		x *= 0x735a2d97U;
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

const mixer32 wang_1 = {
	"mix32::wang_1", [](uint32_t x) {
		x = (x ^ 61) ^ (x >> 16);
		x *= 9;
		x ^= x >> 4;
		x *= 0x27d4eb2dU;
		x ^= x >> 15;
		return x;
	}
};

const mixer32 jenkins = {
	"mix32::jenkins", [](uint32_t x) {
		x = (x + 0x7ed55d16U) + (x << 12);
		x = (x ^ 0xc761c23cU) ^ (x >> 19);
		x = (x + 0x165667b1U) + (x << 5);
		x = (x + 0xd3a2646cU) ^ (x << 9);
		x = (x + 0xfd7046c5U) + (x << 3);
		x = (x ^ 0xb55a4f09U) ^ (x >> 16);
		return x;
	}
};
}


template <>
inline mixer<uint32_t> get_default_mixer() {
	return mix32::mx3;
}

template <>
inline std::vector<mixer32> get_mixers() {
	return {
		mix32::mx1,
		mix32::mx2,
		mix32::mx3,
		mix32::prospector,
		mix32::prospector_boost,
		mix32::murmur,
		mix32::h2_sql,
		mix32::wang_1,
		mix32::jenkins
	};
}
}
