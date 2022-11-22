#pragma once

#include <string>

namespace mixer {

struct config {
	std::string trng_path;
};

const config& get_config();
void set_config(const config&);

}
