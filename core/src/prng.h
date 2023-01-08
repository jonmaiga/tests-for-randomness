#pragma once

#include "stream.h"

namespace mixer {

template <typename T> using prng = stream<T>;

template <typename T>
prng<T> create_prng_from_mixer(mixer<T> mixer, T seed) {
	return {
		"p-" + mixer.name, [state = seed, mixer]() mutable {
			return state = mixer(state);
		}
	};
}

}
