#pragma once

#include <vector>
#include "mixer.h"

namespace mixer {

using mixer32 = mixer<uint32_t>;

namespace mix32 {

const mixer32 mx3 = {
	"mx3", [](uint32_t x) {
		constexpr uint32_t C = 0xeb34556d;
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

const mixer32 sffs_mx3_1 = {
	"mx3_100000", [](uint32_t x) {
		x ^= x >> 18;
		x *= 4276026067;
		x ^= x >> 11;
		x *= 4276026067;
		x ^= x >> 26;
		x *= 4276026067;
		x ^= x >> 29;
		return x;
	}
};

const mixer32 xm3x = {
	"xm3x", [](uint32_t x) {
		constexpr uint32_t C = 2471660141;
		//constexpr uint32_t C = 0b01010101011101010111010101010101;
		//constexpr uint32_t C = 2494157447;
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


const mixer32 xmxmx = {
	"xmxmx", [](uint32_t x) {
		constexpr uint32_t C = 0x9b1a615dull;
		x ^= x >> 16;
		x *= C;
		x ^= x >> 16;
		x *= C;
		x ^= x >> 16;
		return x;
	}
};

const mixer32 xmx = {
	"xmx", [](uint32_t x) {
		constexpr uint32_t C = 0x9b1a615dull;
		x ^= x >> 16;
		x *= C;
		x ^= x >> 16;
		return x;
	}
};

const mixer32 xmx_sffs = {
	"xmx_sffs", [](uint32_t x) {
		x ^= x >> 2;
		x *= 3471659223;
		x ^= x >> 2;

		// x ^= x >> 2;
		// x *= 1312233105;
		// x ^= x >> 1;

		return x;
	}
};

const mixer32 sffs_xmxmx_1 = {
	"xmxmx_sffs_1", [](uint32_t x) {
		x ^= x >> 16;
		x *= 2471660141;
		x ^= x >> 15;
		x *= 2471660141;
		x ^= x >> 14;
		return x;
	}
};

const mixer32 sffs_xmxmx_best = {
	"xmxmx_sffs_BEST", [](uint32_t x) {
		x ^= x >> 16;
		x *= 3714929043;
		x ^= x >> 11;
		x *= 3714929043;
		x ^= x >> 15;
		return x;
	}
};

const mixer32 sffs_xmxmx_best_pr = {
	"xmxmx_sffs_BEST_PR", [](uint32_t x) {
		x ^= x >> 16;
		x *= 3016919661;
		x ^= x >> 15;
		x *= 3016919661;
		x ^= x >> 14;
		return x;
	}
};

const mixer32 murmur = {
	"murmur", [](uint32_t x) {
		x ^= x >> 16;
		x *= 0x85ebca6bU;
		x ^= x >> 13;
		x *= 0xc2b2ae35U;
		x ^= x >> 16;
		return x;
	}
};

const mixer32 wang_1 = {
	"wang_1", [](uint32_t x) {
		x = (x ^ 61) ^ (x >> 16);
		x *= 9;
		x ^= x >> 4;
		x *= 0x27d4eb2d;
		x ^= x >> 15;
		return x;
	}
};

const mixer32 prospector = {
	"prospector", [](uint32_t x) {
		x ^= x >> 15;
		x *= 0x2c1b3c6dU;
		x ^= x >> 12;
		x *= 0x297a2d39U;
		x ^= x >> 15;
		return x;
	}
};

const mixer32 h2_sql = {
	"h2sql", [](uint32_t x) {
		// Thomas Mueller
		x = ((x >> 16) ^ x) * 0x45d9f3bU;
		x = ((x >> 16) ^ x) * 0x45d9f3bU;
		x = (x >> 16) ^ x;
		return x;
	}
};

const mixer32 jenkins = {
	"jenkins", [](uint32_t x) {
		x = (x + 0x7ed55d16) + (x << 12);
		x = (x ^ 0xc761c23c) ^ (x >> 19);
		x = (x + 0x165667b1) + (x << 5);
		x = (x + 0xd3a2646c) ^ (x << 9);
		x = (x + 0xfd7046c5) + (x << 3);
		x = (x ^ 0xb55a4f09) ^ (x >> 16);
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
		//mix32::mx3,
		//mix32::sffs_mx3_1,

		mix32::xmxmx,
		mix32::sffs_xmxmx_1,
		mix32::sffs_xmxmx_best,
		mix32::sffs_xmxmx_best_pr,
		mix32::prospector,
		mix32::murmur,
		mix32::h2_sql,

		mix32::xmx,
		mix32::xmx_sffs,

		mix32::wang_1,
		mix32::jenkins

		//mix32::sffs_xmxmx_1,
		//mix32::sffs_xmxmx_2,
		//mix32::sffs_xmxmx_3,
		//mix32::sffs_xmxmx_4,
	};
}

}
