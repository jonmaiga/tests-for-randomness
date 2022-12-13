#pragma once

#include "util/bitvector.h"

namespace mixer {

using fitness_function = std::function<double(const bit_vector&, unsigned int num_threads)>;
using bit_vector_to_string = std::function<std::string(const bit_vector&)>;

struct sffs_config {
	int bits;
	fitness_function fitness;
	bit_vector_to_string to_string;
	bit_vector_to_string to_arr_str;
	std::optional<bit_vector> seed;
	int min = 0;
	int max = 1000;
};

}
