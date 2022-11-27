#pragma once

#include "util/bitwise.h"
#include "types.h"

namespace mixer {

using mixer64 = mixer<uint64_t>;

template <typename T>
mixer<T> create_mixer_from_stream(const std::string& name, stream<T> source) {
	return {
		name, [source](T) mutable {
			return source();
		}
	};
}

namespace mix64 {

const mixer64 mx3 = {
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

const mixer64 xmxmx = {
	"xmxmx", [](uint64_t x) {
		constexpr uint64_t C = 0xe9846af9b1a615dull;
		x ^= x >> 32;
		x *= C;
		x ^= x >> 32;
		x *= C;
		x ^= x >> 28;
		return x;
	}
};


const mixer64 xmx = {
	"xmx", [](uint64_t x) {
		x ^= x >> 32;
		x *= 0xe9846af9b1a615dull;
		x ^= x >> 29;
		return x;
	}
};


const mixer64 murmur3 = {
	"murmur3", [](uint64_t x) {
		x ^= x >> 33;
		x *= 0xff51afd7ed558ccd;
		x ^= x >> 33;
		x *= 0xc4ceb9fe1a85ec53;
		x ^= x >> 33;
		return x;
	}
};


const mixer64 split_mix = {
	"splitmix", [](uint64_t x) {
		x ^= x >> 30;
		x *= 0xbf58476d1ce4e5b9;
		x ^= x >> 27;
		x *= 0x94d049bb133111eb;
		x ^= x >> 31;
		return x;
	}
};


const mixer64 nasam = {
	"nasam", [](uint64_t x) {
		x ^= ror(x, 25) ^ ror(x, 47);
		x *= 0x9E6C63D0676A9A99UL;
		x ^= x >> 23 ^ x >> 51;
		x *= 0x9E6D62D06F6A9A9BUL;
		x ^= x >> 23 ^ x >> 51;
		return x;
	}
};

const mixer64 moremur = {
	"moremur", [](uint64_t x) {
		x ^= x >> 27;
		x *= 0x3C79AC492BA7B653UL;
		x ^= x >> 33;
		x *= 0x1C69B3F74AC4AE35UL;
		x ^= x >> 27;
		return x;
	}
};

const mixer64 fast_hash = {
	"fast_hash", [](uint64_t x) {
		x ^= (x >> 23);
		x *= 0x2127599bf4325c37ull;
		x ^= (x >> 47);
		return x;
	}
};

const mixer64 xxh3 = {
	"xxh3", [](uint64_t x) {
		x ^= x >> 37;
		x *= 0x165667919E3779F9ULL;
		x ^= x >> 32;
		return x;
	}
};

const mixer64 lea64 = {
	"lea64", [](uint64_t x) {
		x ^= (x >> 32);
		x *= 0xdaba0b6eb09322e3ull;
		x ^= (x >> 32);
		x *= 0xdaba0b6eb09322e3ull;
		return x ^ (x >> 32);
	}
};

const mixer64 degski64 = {
	"degski64", [](uint64_t x) {
		x ^= (x >> 32);
		x *= 0xd6e8feb86659fd93ull;
		x ^= (x >> 32);
		x *= 0xd6e8feb86659fd93ull;
		return x ^ (x >> 32);
	}
};

}

template <>
inline std::vector<mixer64> get_mixers() {
	return {
		mix64::mx3,
		mix64::nasam,
		mix64::xmxmx,
		mix64::moremur,
		mix64::lea64,
		mix64::degski64,
		mix64::split_mix,
		mix64::murmur3,
		mix64::xmx,
		mix64::xxh3,
		mix64::fast_hash
	};
}

}
