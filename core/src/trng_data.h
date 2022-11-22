#pragma once

#include <iostream>

#include "config.h"
#include "util/fileutil.h"

namespace mixer {

template <typename T>
const std::vector<T>& get_trng_data() {
	static const auto trng_data = []() {
		std::cout << "Reading trng stream from disk...";
		const auto& data = readBinaryMustExist<T>(get_config().trng_path);
		std::cout << " done!\n";
		return data;
	}();

	return trng_data;
}

}
