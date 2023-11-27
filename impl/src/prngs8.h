#pragma once
#include "prngs64.h"

namespace tfr {
using prng8 = prng<uint8_t>;

namespace rng8 {
inline prng8 splitmix_64(const seed_data& seed) {
	return {
		"rng8::splitmix_64", [rng_64 = rng64::splitmix(seed)]() mutable {
			return static_cast<uint8_t>(rng_64() >> 56);
		}
	};
}

inline prng8 pcg_64(const seed_data& seed) {
	return {
		"rng8::pcg_64", [rng_64 = rng64::pcg(seed)]() mutable {
			return static_cast<uint8_t>(rng_64());
		}
	};
}
}


template <>
inline std::vector<prng_factory<uint8_t>> get_prngs() {
	return {
		rng8::splitmix_64,
		rng8::pcg_64
	};
}
}
