#pragma once
#include "prngs32.h"

namespace tfr {
using prng16 = prng<uint16_t>;

namespace rng16 {
inline prng16 splitmix_64(const seed_data& seed) {
	return {
		"rng16::splitmix_64", [rng_64 = rng64::splitmix(seed)]() mutable {
			return static_cast<uint16_t>(rng_64() >> 48);
		}
	};
}

inline prng16 pcg_64(const seed_data& seed) {
	return {
		"rng16::pcg_64", [rng_64 = rng64::pcg(seed)]() mutable {
			return static_cast<uint16_t>(rng_64());
		}
	};
}
}


template <>
inline std::vector<prng_factory<uint16_t>> get_prngs() {
	return {
		rng16::splitmix_64,
		rng16::pcg_64,
	};
}
}
