#pragma once

#include "mixers32.h"
#include "stream.h"

namespace mixer {
    
namespace prng32 {

stream<uint32_t> xmx(uint32_t seed) {
    return {"xmx", [state = seed]() mutable {
        return state = mix32::xmx(state);
    }};
}

stream<uint32_t> xm2x(uint32_t seed) {
    return {"xm2x", [state = seed]() mutable {
        return mix32::xm2x(++state);
    }};
}

stream<uint32_t> xm3x(uint32_t seed) {
    return {"xm3x", [state = seed]() mutable {
        return mix32::xm3x(++state);
    }};
}

}

//template <>
inline std::vector<stream<uint32_t>> get_prngs(uint32_t seed) {
	return {
        prng32::xmx(seed),
        prng32::xm2x(seed),
		prng32::xm3x(seed),
	};
}

}
