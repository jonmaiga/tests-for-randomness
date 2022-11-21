#pragma once

#include "util/fileutil.h"

namespace mixer {

template <typename T>
const std::vector<T>& get_trng_data() {
	static const auto trng_data = []() {
		std::cout << "Reading trng stream from disk...";
		const auto& data = readBinaryMustExist<T>(R"(C:\tmp\random.org\trng.bin)");
		std::cout << " done!\n";
		return data;
	}();

	return trng_data;
}

}
