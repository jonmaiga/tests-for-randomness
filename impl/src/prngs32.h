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

inline prng32 xm3x(uint32_t seed) {
	return {
		"rng32::xm3x", [state = seed]() mutable {
			return state = mix32::xm3x(state);
		}
	};
}

inline prng32 xm2x(uint32_t seed) {
	return {
		"rng32::xm2x", [state = seed]() mutable {
			return state = mix32::xm2x(state);
		}
	};
}

inline prng32 xmx(uint32_t seed) {
	return {
		"rng32::xmx", [state = seed]() mutable {
			return state = mix32::xmx(state);
		}
	};
}

inline prng32 pcg(uint32_t seed) {
	pcg32 pcg(seed);
	return {
		"rng32::pcg-" + std::to_string(seed), [pcg]() mutable {
			return pcg();
		}
	};
}

inline prng32 xoshiro128plusplus(uint32_t seed) {
	// https://prng.di.unimi.it/xoshiro128plusplus.c
	uint32_t s[4]{seed, seed, seed, seed};
	return {
		"rng32::xoshiro128++", [s]() mutable {

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

inline prng32 xorshift(uint32_t seed) {
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
	return {
		"rng32::xorshift", [x = seed]() mutable {
			x ^= x << 13;
			x ^= x >> 17;
			x ^= x << 5;
			return x;
		}
	};
}

inline prng32 mt19337(uint32_t seed) {
	std::mt19937 gen(seed);
	return {
		"rng32::mt19937", [gen]() mutable {
			return gen();
		}
	};
}

inline prng32 minstd_rand(uint32_t seed) {
	std::minstd_rand gen(seed);
	return {
		"rng32::minstd_rand", [gen]() mutable {
			return gen();
		}
	};
}

inline prng32 rdrand(uint32_t) {
	return {
		"rng32::rdrand", []() mutable {
			return rdrand_32();
		}
	};
}

inline prng32 aes(uint32_t seed) {
	return {
		"rng32::aes", [state = seed]() mutable {
			for (int i = 0; i < 1; ++i) {
				state = aes_mix(state);
			}
			return state;
		}
	};
}

inline prng32 aes128(uint32_t seed) {
	std::array state{seed, seed, seed, seed};
	return {
		"rng32::aes128", [state]() mutable {
			return aes_prng128(state);
		}
	};
}

}


template <>
inline std::vector<prng_factory<uint32_t>> get_prngs() {
	return {
		rng32::xm3x,
		rng32::xm2x,
		rng32::xmx,
		rng32::pcg,
		rng32::xoshiro128plusplus,
		rng32::mt19337,
		rng32::xorshift,
		rng32::minstd_rand,
	};
}


}
