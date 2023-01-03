#pragma once

#include "external/pcg/pcg_random.hpp"
#include "mixers32.h"
#include "stream.h"

namespace mixer {
    
namespace prng32 {

stream<uint32_t> xmx(uint32_t seed) {
    return {"xmx", [state = seed]() mutable {
        return state = mix32::xmx(state);
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


}

template <typename T> std::vector<stream<T>> get_prngs(T seed) = delete;

template <>
inline std::vector<stream<uint32_t>> get_prngs(uint32_t seed) {
	return {
        //prng32::xmx(seed),
        //prng32::pcg(seed),
        prng32::xoroshift(seed),
	};
}

}
