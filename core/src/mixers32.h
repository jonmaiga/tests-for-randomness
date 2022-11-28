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

const mixer32 sffs_mx3_2 = {
	"sffs_mx3", [](uint32_t x) {
		x ^= x >> 16;
		x *= 20493;
		x ^= x >> 16;
		x *= 20493;
		x ^= x >> 8;
		x *= 20493;
		x ^= x >> 8;
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

const mixer32 xmxmx = {
	"xmxmx", [](uint32_t x) {
		constexpr uint64_t C = 0x9b1a615dull;
		x ^= x >> 16;
		x *= C;
		x ^= x >> 16;
		x *= C;
		x ^= x >> 16;
		return x;
	}
};


const mixer32 sffs_xmxmx_1 = {
	"xmxmx_20000_trng", [](uint32_t x) {
		// 18, 13, 16, 388373931 ] : 13.0175 vs 38.0374
		// 18, 13, 16, 392568235
		x ^= x >> 18;
		x *= 392568235;
		x ^= x >> 13;
		x *= 392568235;
		x ^= x >> 16;
		return x;
	}
};

const mixer32 sffs_xmxmx_2 = {
	"xmxmx_2", [](uint32_t x) {
		x ^= x >> 14;
		x *= 4155717301;
		x ^= x >> 13;
		x *= 4155717301;
		x ^= x >> 14;
		return x;
	}
};

const mixer32 sffs_xmxmx_3 = {
	"xmxmx_200000_c1_gc", [](uint32_t x) {
		x ^= x >> 15;
		x *= 2232101479;
		x ^= x >> 12;
		x *= 2232101479;
		x ^= x >> 18;
		return x;
	}
};

const mixer32 sffs_xmxmx_4 = {
	"xmxmx_200000_trng", [](uint32_t x) {
		x ^= x >> 16;
		x *= 1647422747;
		x ^= x >> 16;
		x *= 1647422747;
		x ^= x >> 16;
		return x;
	}
};

}

template <>
inline std::vector<mixer32> get_mixers() {
	return {
		mix32::mx3,
		//mix32::sffs_mx3_1,
		//mix32::sffs_mx3_2,

		mix32::xmxmx,
		mix32::prospector,
		//mix32::murmur,
		//mix32::h2_sql,
		//mix32::sffs_xmxmx_1,
		mix32::sffs_xmxmx_2,
		//mix32::sffs_xmxmx_3,
		//mix32::sffs_xmxmx_4,
	};
}

}
