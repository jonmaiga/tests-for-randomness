#pragma once

#include "external/pcg/pcg_random.hpp"
#include "mixers32.h"
#include "stream.h"

#include <random>

namespace mixer {
    
namespace prng32 {

stream<uint32_t> xmx(uint32_t seed) {
    return {"xmx", [state = seed]() mutable {
        return state = mix32::xmx(state);
    }};
}

stream<uint32_t> xm2x(uint32_t seed) {
    return {"xm2x", [state = seed]() mutable {
        return state = mix32::xm2x(state);
    }};
}

stream<uint32_t> xm3x(uint32_t seed) {
    return {"xm3x", [state = seed]() mutable {
        return state = mix32::xm3x(state);
    }};
}


stream<uint32_t> pcg(uint32_t seed) {
    pcg32 pcg(seed);
    return {"pcg", [pcg]() mutable {
        return static_cast<uint32_t>(pcg());
    }};
}

stream<uint32_t> xoroshift(uint32_t seed) {
    return {"xoroshift", [x = seed]() mutable {
        /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        return x;        
    }};
}

stream<uint32_t> mt(uint32_t seed) {
    std::mt19937 gen(seed);
    return {"mt", [gen]() mutable {
        return gen();
    }};
}


}

template <typename T> std::vector<stream<T>> get_prngs(T seed) = delete;

template <>
inline std::vector<stream<uint32_t>> get_prngs(uint32_t seed) {
	return {
        prng32::mt(seed),
        //prng32::xmx(seed), // 29 uniform p=1.0
        //prng32::xm2x(seed), // 29
        //prng32::xm3x(seed), // 29
        //prng32::pcg(seed), // 29
        //prng32::xoroshift(seed), // 29 uniform p=1.0
	};
}

}
