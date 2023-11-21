#pragma once

#include "stream.h"
#include <array>

namespace tfr {
struct seed_data {
	uint16_t s16(size_t i = 0) const { return static_cast<uint16_t>(data[i]); }
	uint32_t s32(size_t i = 0) const { return static_cast<uint32_t>(data[i]); }
	uint64_t s64(size_t i = 0) const { return data[i]; }

	std::array<uint32_t, 4> s128_32() const { return {s32(0), s32(1), s32(2), s32(3)}; }
	std::array<uint64_t, 2> s128_64() const { return {s64(0), s64(1)}; }
	std::array<uint64_t, 4> s256_64() const { return data; }

	std::array<uint64_t, 4> data;
};

template <typename T> using prng = stream<T>;

template <typename T> using prng_factory = std::function<prng<T>(seed_data)>;

template <typename T> std::vector<prng_factory<T>> get_prngs() = delete;

template <typename T>
prng<T> create_prng_from_mixer(mixer<T> mixer, T seed) {
	return {
		"p-" + mixer.name, [state = seed, mixer]() mutable {
			return state = mixer(state);
		}
	};
}
}
