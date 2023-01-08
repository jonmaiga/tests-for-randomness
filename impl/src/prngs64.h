#pragma once

#include "mixers64.h"
#include "stream.h"

namespace mixer {

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

inline prng64 xoroshiroplus(uint64_t seed) {
	uint64_t s[]{seed, seed};
	return {
		"xoroshiroplus", [s]() mutable {
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
inline std::vector<prng64> get_prngs(uint64_t seed) {
	return {
		rng64::pcg(seed), // >31 suspicous (4)
		//rng64::xoroshiroplus(seed), // >31
		//rng64::xmx(seed), // >31
		//rng64::xm2x(seed), // >31
		//rng64::xm3x(seed), // >31
	};
}

}
