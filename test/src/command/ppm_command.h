#pragma once

#include "util/ppm.h"

namespace mixer {

template <typename T>
void ppm_command() {
	constexpr int size = 512;
	auto trng_stream = create_stream_from_data_by_ref<T>("trng", get_trng_data<T>());
	write_ppm<T>(
		get_config().root_path + "ppm/" + trng_stream.name + ".ppm",
		size, size,
		[trng_stream](T, T) mutable { return trng_stream(); });

	for (const auto& mix : get_mixers<T>()) {

		write_ppm<T>(
			get_config().root_path + "ppm/" + mix.name + ".ppm",
			size, size,
			[&mix](T x, T y) { return mix.mix(y * size + x); });
	}
}

}
