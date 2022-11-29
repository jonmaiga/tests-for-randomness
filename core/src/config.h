#pragma once

#include <string>

namespace mixer {

struct config {
	std::string root_path;

	std::string trng_path() const {
		return root_path + "/data/trng.bin";
	}

	std::string result_path() const {
		return root_path + "/result.txt";
	}
};

const config& get_config();
void set_config(const config&);

}
