#pragma once

#include "bit_vector.h"

namespace mixer {

using fitness_function = std::function<double(const bit_vector&)>;
using bit_vector_to_string = std::function<std::string(const bit_vector&)>;

struct config {
	int bits;
	fitness_function fitness;
	bit_vector_to_string to_string;
	bit_vector_to_string to_arr_str;
	std::optional<bit_vector> seed;
};

}
