#pragma once

#include "util/fileutil.h"

namespace mixer {

inline const std::vector<uint64_t>& get_trng_data() {
	static const auto trng_data = []() {
		std::cout << "Reading trng stream from disk...";
		const auto& data = readBinaryMustExist<uint64_t>(R"(C:\tmp\random.org\trng.bin)");
		std::cout << " done!\n";
		return data;
	}();

	return trng_data;
}

}