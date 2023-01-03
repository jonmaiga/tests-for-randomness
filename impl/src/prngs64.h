#pragma once

#include "stream.h"

namespace mixer {

namespace prng64 {

stream<uint64_t> xoroshiftplus(uint64_t seed) {
    uint64_t s[]{seed, seed};
    return {"xoroshiftplus", [s]() mutable {
        const uint64_t s0 = s[0];
        uint64_t s1 = s[1];
        const uint64_t result = s0 + s1;

        s1 ^= s0;
        s[0] = rol(s0, 55) ^ s1 ^ (s1 << 14); // a, b
        s[1] = rol(s1, 36); // c
        return result;   
    }};
}

}

template<>
inline std::vector<stream<uint64_t>> get_prngs(uint64_t seed) {
	return {
        prng64::xoroshiftplus(seed),
	};
}

}