#pragma once

#include "external/pcg/pcg_random.hpp"
#include "external/rdrand/rdrand.h"
#include "external/aes/aes.h"
#include "mixers32.h"
#include "prng.h"

#include <array>
#include <random>

#include "util/bitwise.h"

namespace tfr {
using prng32 = prng<uint32_t>;

namespace rng32 {
inline prng32 mx1(const seed_data& seed) {
	return {
		"rng32::mx1", [state = seed.s32()]() mutable {
			state += 2471660141U;
			return mix32::mx1(state);
		}
	};
}

inline prng32 mx2(const seed_data& seed) {
	return {
		"rng32::mx2", [state = seed.s32()]() mutable {
			state += 1159349557U;
			return mix32::mx2(state);
		}
	};
}

inline prng32 mx3(const seed_data& seed) {
	return {
		"rng32::mx3", [state = seed.s32()]() mutable {
			state += 1159349557U;
			return mix32::mx3(state);
		}
	};
}

inline prng32 pcg_64(const seed_data& seed) {
	pcg32 pcg(seed.s64());
	return {
		"rng32::pcg_64", [pcg]() mutable {
			return pcg();
		}
	};
}

inline prng32 xoshiro128plusplus_128(const seed_data& seed) {
	// https://prng.di.unimi.it/xoshiro128plusplus.c
	return {
		"rng32::xoshiro128++_128", [s = seed.s128_32()]() mutable {
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

inline prng32 xorshift(const seed_data& seed) {
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
	return {
		"rng32::xorshift", [x = seed.s32()]() mutable {
			x ^= x << 13;
			x ^= x >> 17;
			x ^= x << 5;
			return x;
		}
	};
}

inline prng32 mt19337(const seed_data& seed) {
	std::mt19937 gen(seed.s32());
	return {
		"rng32::mt19937", [gen]() mutable {
			return gen();
		}
	};
}

inline prng32 minstd_rand(const seed_data& seed) {
	std::minstd_rand gen(seed.s32());
	return {
		"rng32::minstd_rand", [gen]() mutable {
			return gen();
		}
	};
}

inline prng32 rdrand(const seed_data&) {
	return {
		"rng32::rdrand", []() mutable {
			return rdrand_32();
		}
	};
}

inline prng32 aes(const seed_data& seed) {
	return {
		"rng32::aes", [state = seed.s32()]() mutable {
			for (int i = 0; i < 1; ++i) {
				state = aes_mix(state);
			}
			return state;
		}
	};
}

inline prng32 aes_128(const seed_data& seed) {
	return {
		"rng32::aes_128", [state = seed.s128_32()]() mutable {
			return aes_prng128(state);
		}
	};
}
}


template <>
inline std::vector<prng_factory<uint32_t>> get_prngs() {
	return {
		rng32::mx1,
		rng32::mx2,
		rng32::mx3,
		rng32::pcg_64,
		rng32::xoshiro128plusplus_128,
		rng32::mt19337,
		rng32::xorshift,
		rng32::minstd_rand,
	};
}
}
