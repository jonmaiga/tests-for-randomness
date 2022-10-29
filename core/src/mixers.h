#pragma once

#include "util/bitwise.h"
#include "types.h"

namespace mixer {


inline mixer create_mixer_from_stream(const std::string& name, const stream& source) {
	return {
		name, [&source](uint64_t) {
			return source();
		}
	};
}

const mixer mx3 = {
	"mx3", [](uint64_t x) {
		constexpr uint64_t C = 0xbea225f9eb34556d;
		x ^= (x >> 32);
		x *= C;
		x ^= (x >> 29);
		x *= C;
		x ^= (x >> 32);
		x *= C;
		x ^= (x >> 29);
		return x;
	}
};

const mixer xmxmxm = {
	"xmxmxm", [](uint64_t x) {
		constexpr uint64_t C = 0xe9846af9b1a615dull;
		x ^= x >> 32;
		x *= C;
		x ^= x >> 32;
		x *= C;
		x ^= x >> 28;
		return x;
	}
};


const mixer xmx = {
	"xmx", [](uint64_t x) {
		x ^= x >> 32;
		x *= 0xe9846af9b1a615dull;
		x ^= x >> 29;
		return x;
	}
};


const mixer murmur3 = {
	"murmur3", [](uint64_t x) {
		x ^= x >> 33;
		x *= 0xff51afd7ed558ccd;
		x ^= x >> 33;
		x *= 0xc4ceb9fe1a85ec53;
		x ^= x >> 33;
		return x;
	}
};

const mixer nasam = {
	"nasam", [](uint64_t x) {
		x ^= ror64(x, 25) ^ ror64(x, 47);
		x *= 0x9E6C63D0676A9A99UL;
		x ^= x >> 23 ^ x >> 51;
		x *= 0x9E6D62D06F6A9A9BUL;
		x ^= x >> 23 ^ x >> 51;
		return x;
	}
};

const mixer moremur = {
	"moremur", [](uint64_t x) {
		x ^= x >> 27;
		x *= 0x3C79AC492BA7B653UL;
		x ^= x >> 33;
		x *= 0x1C69B3F74AC4AE35UL;
		x ^= x >> 27;
		return x;
	}
};

const mixer fast_hash = {
	"fast_hash", [](uint64_t x) {
		x ^= (x >> 23);
		x *= 0x2127599bf4325c37ull;
		x ^= (x >> 47);
		return x;
	}
};

const mixer xxh3 = {
	"xxh3", [](uint64_t x) {
		x ^= x >> 37;
		x *= 0x165667919E3779F9ULL;
		x ^= x >> 32;
		return x;
	}
};

const mixer lea64 = {
	"lea64", [](uint64_t x) {
		x ^= (x >> 32);
		x *= 0xdaba0b6eb09322e3ull;
		x ^= (x >> 32);
		x *= 0xdaba0b6eb09322e3ull;
		return x ^ (x >> 32);
	}
};

const mixer degski64 = {
	"degski64", [](uint64_t x) {
		x ^= (x >> 32);
		x *= 0xd6e8feb86659fd93ull;
		x ^= (x >> 32);
		x *= 0xd6e8feb86659fd93ull;
		return x ^ (x >> 32);
	}
};

}
