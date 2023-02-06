#pragma once

#include "stream.h"

namespace tfr {

template <typename T> using prng = stream<T>;

template <typename T> using prng_factory = std::function<prng<T>(T seed)>;

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
