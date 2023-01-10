#pragma once

#include "external/pcg/pcg_random.hpp"
#include "mixers32.h"
#include "prng.h"

#include <random>

#include "util/bitwise.h"

namespace mixer {

using prng32 = prng<uint32_t>;

namespace rng32 {

inline prng32 xmx(uint32_t seed) {
	return {
		"xmx", [state = seed]() mutable {
			return state = mix32::xmx(state);
		}
	};
}

inline prng32 xm2x(uint32_t seed) {
	return {
		"xm2x", [state = seed]() mutable {
			return state = mix32::xm2x(state);
		}
	};
}

inline prng32 xm3x(uint32_t seed) {
	return {
		"xm3x", [state = seed]() mutable {
			return state = mix32::xm3x(state);
		}
	};
}

inline prng32 pcg(uint32_t seed) {
	pcg32 pcg(seed);
	return {
		"pcg-" + std::to_string(seed), [pcg]() mutable {
			return pcg();
		}
	};
}

inline prng32 xorshift(uint32_t seed) {
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
	return {
		"xorshift", [x = seed]() mutable {
			x ^= x << 13;
			x ^= x >> 17;
			x ^= x << 5;
			return x;
		}
	};
}

inline prng32 xoshiro128plusplus(uint32_t seed) {
	// https://prng.di.unimi.it/xoshiro128plusplus.c
	uint32_t s[4]{seed, seed, seed, seed};
	return {
		"xoshiro128plusplus", [s]() mutable {

			const uint32_t result = rol(s[0] + s[3], 7) + s[0];
			const uint32_t t = s[1] << 9;

			s[2] ^= s[0];
			s[3] ^= s[1];
			s[1] ^= s[2];
			s[0] ^= s[3];

			s[2] ^= t;

			s[3] = rol(s[3], 11);

			return result;
		}
	};
}


inline prng32 mt19337(uint32_t seed) {
	std::mt19937 gen(seed);
	return {
		"mt", [gen]() mutable {
			return gen();
		}
	};
}

inline prng32 minstd_rand(uint32_t seed) {
	std::minstd_rand gen(seed);
	return {
		"minstd_rand", [gen]() mutable {
			return gen();
		}
	};
}

}

template <typename T> std::vector<stream<T>> get_prngs(T seed) = delete;

template <>
inline std::vector<prng32> get_prngs(uint32_t seed) {
	return {
		rng32::mt19337(seed), // >30 ww-s172       very suspicious (8)
		rng32::xmx(seed), // 24 bcc
		rng32::xm2x(seed), // 29 uniform-m     failure (10) xm2x(p=0.000000) 
		rng32::xm3x(seed), // 26, bcc, uniform
		rng32::pcg(seed), // > 35
		rng32::minstd_rand(seed), // 10 mean, uniform, gap, coupon
		rng32::xorshift(seed), // 
		rng32::xoshiro128plusplus(seed), //  >34 (only bcc)
	};
}

}
