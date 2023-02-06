#pragma once

#include "util/ppm.h"

namespace tfr {

template <typename T>
void ppm_command() {
	const auto dir = get_config().root_path + "ppm" + std::to_string(bit_sizeof<T>()) + "/";
	auto trng_stream = create_stream_from_data_by_ref<T>("trng", get_trng_data<T>());
	write_ppm(dir + trng_stream.name + ".ppm", trng_stream);

	for (const auto& mix : get_mixers<T>()) {
		constexpr int size = 512;
		write_ppm<T>(
			dir + mix.name + ".ppm",
			size, size,
			[&mix](T x, T y) { return mix.mix(y * size + x); });
	}
}

}
