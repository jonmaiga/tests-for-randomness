#pragma once

#include "mixers64.h"
#include "stream.h"

namespace tfr {

using prng64 = prng<uint64_t>;

namespace rng64 {

inline prng64 xmx(uint64_t seed) {
	return {
		"xmx", [state = seed]() mutable {
			return state = mix64::xmx(state);
		}
	};
}

inline prng64 xm2x(uint64_t seed) {
	return {
		"xm2x", [state = seed]() mutable {
			return state = mix64::xm2x(state);
		}
	};
}

inline prng64 xm3x(uint64_t seed) {
	return {
		"xm3x", [state = seed]() mutable {
			return state = mix64::mx3(state);
		}
	};
}

inline prng64 pcg(uint64_t seed) {
	pcg64 pcg(seed);
	return {
		"pcg", [pcg]() mutable {
			return pcg();
		}
	};
}

inline prng64 xorshift128plus(uint64_t seed) {
	uint64_t s[]{seed, seed};
	return {
		"xorshift128+", [s]() mutable {
			// from https://lemire.me/blog/2017/09/08/the-xorshift128-random-number-generator-fails-bigcrush/
			uint64_t s1 = s[0];
			const uint64_t s0 = s[1];
			const uint64_t result = s0 + s1;
			s[0] = s0;
			s1 ^= s1 << 23; // a
			s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5); // b, c
			return result;
		}
	};
}

inline prng64 xoroshiro128plus(uint64_t seed) {
	uint64_t s[]{seed, seed};
	return {
		"xoroshiro128+", [s]() mutable {
			// from https://prng.di.unimi.it/xoroshiro128plus.c
			const uint64_t s0 = s[0];
			uint64_t s1 = s[1];
			const uint64_t result = s0 + s1;

			s1 ^= s0;
			s[0] = rol(s0, 55) ^ s1 ^ (s1 << 14); // a, b
			s[1] = rol(s1, 36); // c
			return result;
		}
	};
}

}

template <>
inline std::vector<prng_factory<uint64_t>> get_prngs() {
	return {
		rng64::pcg,
		rng64::xoroshiro128plus,
		rng64::xorshift128plus,
		rng64::xmx,
		rng64::xm2x,
		rng64::xm3x,
	};
}

}
