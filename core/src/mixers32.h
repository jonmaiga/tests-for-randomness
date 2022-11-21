#pragma once

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

const mixer32 sffs1 = {
	"sffs_all", [](uint32_t x) {
		x ^= x >> 11;
		x *= 708060363;
		x ^= x >> 14;
		x *= 708060363;
		x ^= x >> 15;
		return x;
	}
};

const mixer32 sffs2 = {
	"sffs_bic", [](uint32_t x) {
		x ^= x >> 16;
		x *= 12987573;
		x ^= x >> 16;
		x *= 12987573;
		x ^= x >> 12;
		return x;
	}
};

const mixer32 sffs3 = {
	"sffs_mean_chi2", [](uint32_t x) {
		x ^= x >> 30;
		x *= 4274575107;
		x ^= x >> 31;
		x *= 4274575107;
		x ^= x >> 24;
		return x;
	}
};
const mixer32 sffs4 = {
	"sffs_mean_chi2_ww", [](uint32_t x) {
		x ^= x >> 16;
		x *= 4020289823;
		x ^= x >> 10;
		x *= 4020289823;
		x ^= x >> 17;
		return x;
	}
};

const mixer32 sffs5 = {
	"sffs_many", [](uint32_t x) {
		x ^= x >> 17;
		x *= 688330889;
		x ^= x >> 10;
		x *= 688330889;
		x ^= x >> 17;
		return x;
	}
};

}

template <>
inline std::vector<mixer32> get_mixers() {
	return {
		mix32::mx3,
		mix32::prospector,
		mix32::murmur,
		mix32::h2_sql,
		mix32::sffs1,
		mix32::sffs2,
		mix32::sffs3,
		mix32::sffs4,
		mix32::sffs5,
	};
}

}
