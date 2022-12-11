#pragma once

#include "util/ppm.h"

namespace mixer {

template <typename T>
void ppm_command() {
	for (const auto& mix : get_mixers<T>()) {
		constexpr int size = 256;
		write_ppm<T>(
			get_config().root_path + "ppm/" + mix.name + ".ppm",
			size, size,
			[&mix](T x, T y) { return mix.mix(y * size + x); });
	}
}

}
