#pragma once

#include "util/bitwise.h"
#include <string>

namespace tfr {

struct config {
	std::string root_path;

	std::string trng_path() const {
		return data_path() + "trng.bin";
	}

	std::string drng_path() const {
		return data_path() + "drng.bin";
	}

	std::string result_path() const {
		return root_path;
	}

	template <typename T>
	std::string ppm_dir() const {
		return root_path + "ppm" + std::to_string(bit_sizeof<T>()) + "/";
	}

	std::string data_path() const {
		return root_path + "data/";
	}
};

const config& get_config();
void set_config(const config&);

}
