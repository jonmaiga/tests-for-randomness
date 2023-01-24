#pragma once

#include <array>
#include <cstdint>

namespace mixer {

uint32_t aes_mix(uint32_t);
uint64_t aes_mix(uint64_t);

uint32_t aes_prng128(std::array<uint32_t, 4>& state);

}
