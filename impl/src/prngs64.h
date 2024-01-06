#pragma once

#include "mixers64.h"
#include "prng.h"
#include "stream.h"
#include "external/pcg/pcg_random.hpp"
#include "external/sfc/sfc64.h"

namespace tfr {
using prng64 = prng<uint64_t>;

namespace rng64 {
inline prng64 mx1(const seed_data& seed) {
	return {
		"rng64::mx1", [state = seed.s64()]() mutable {
			state += 0xe9846af9b1a615d;
			return mix64::mx1(state);
		}
	};
}

inline prng64 mx2(const seed_data& seed) {
	return {
		"rng64::mx2", [state = seed.s64()]() mutable {
			state += 0x7574708ca19c768b;
			return mix64::mx2(state);
		}
	};
}

inline prng64 mx3(const seed_data& seed) {
	return {
		"rng64::mx3", [state = seed.s64()]() mutable {
			state += 0xbea225f9eb34556d;
			return mix64::mx3(state);
		}
	};
}

inline prng64 splitmix(const seed_data& seed) {
	return {
		"rng64::splitmix", [state=seed.s64()]() mutable {
			state += 0x9E3779B97F4A7C15;
			uint64_t x = state;
			// David Stafford's Mix13 for MurmurHash3's 64-bit finalizer
			x ^= x >> 30;
			x *= 0xBF58476D1CE4E5B9;
			x ^= x >> 27;
			x *= 0x94D049BB133111EB;
			x ^= x >> 31;
			return x;
		}
	};
}

inline prng64 pcg(const seed_data& seed) {
	pcg64_once_insecure pcg(seed.s64());
	return {
		"rng64::pcg", [pcg]() mutable {
			return pcg();
		}
	};
}

inline prng64 xorshift128plus_128(const seed_data& seed) {
	return {
		"rng64::xorshift128+_128", [s = seed.s128_64()]() mutable {
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

inline prng64 xoroshiro128plus_128(const seed_data& seed) {
	return {
		"rng64::xoroshiro128+_128", [s = seed.s128_64()]() mutable {
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

inline prng64 sfc_256(const seed_data& seed) {
	return {
		"rng64::sfc_256", [rng = sfc64(seed.s64())]() mutable {
			return rng();
		}
	};
}

}

template <>
inline std::vector<prng_factory<uint64_t>> get_prngs() {
	return {
		rng64::mx1,
		rng64::mx2,
		rng64::mx3,
		rng64::splitmix,
		rng64::pcg,
		rng64::xorshift128plus_128,
		rng64::xoroshiro128plus_128,
		rng64::sfc_256,
	};
}
}
