#pragma once

#include "util/bitwise.h"
#include <string>

namespace tfr {

struct config {
	std::string root_path;

	std::string data_dir() const {
		return root_path + "data/";
	}

	std::string result_dir() const {
		return root_path;
	}

	template <typename T>
	std::string test_result_file_path() const {
		return result_dir() + "result_" + std::to_string(bit_sizeof<T>()) + ".md";
	}

	template <typename T>
	std::string ppm_dir() const {
		return result_dir() + "ppm" + std::to_string(bit_sizeof<T>()) + "/";
	}

	std::string trng_file_path() const {
		return data_dir() + "trng.bin";
	}

	std::string drng_file_path() const {
		return data_dir() + "drng.bin";
	}
};

const config& get_config();
void set_config(const config&);
bool is_debug();

}
