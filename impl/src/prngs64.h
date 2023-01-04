#pragma once

#include "mixers64.h"
#include "stream.h"

namespace mixer {

namespace prng64 {

stream<uint64_t> xmx(uint64_t seed) {
    return {"xmx", [state = seed]() mutable {
        return state = mix64::xmx(state);
    }};
}

stream<uint64_t> xm2x(uint64_t seed) {
    return {"xm2x", [state = seed]() mutable {
        return state = mix64::xm2x(state);
    }};
}

stream<uint64_t> xm3x(uint64_t seed) {
    return {"xm3x", [state = seed]() mutable {
        return state = mix64::mx3(state);
    }};
}

stream<uint64_t> pcg(uint64_t seed) {
    pcg64 pcg(seed);
    return {"pcg", [pcg]() mutable {
        return pcg();
    }};
}

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
        prng64::pcg(seed), // >31 suspicous (4)
        //prng64::xoroshiftplus(seed), // >31
        //prng64::xmx(seed), // >31
        //prng64::xm2x(seed), // >31
        //prng64::xm3x(seed), // >31
	};
}

}