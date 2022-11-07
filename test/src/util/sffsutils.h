#pragma once

#include <iostream>
#include <sstream>

#include "sffs.h"

namespace mixer {

inline std::string to_string(const sffs_state& s, const bit_vector_to_string& to_arr_str) {
	std::stringstream ss;
	ss << to_string(s.data);
	if (to_arr_str) {
		ss << " [  " << to_arr_str(s.data) << " ]";
	}
	ss << " : " << s.score;
	return ss.str();
}

inline auto create_sffs_printer(const bit_vector_to_string& to_arr_str) {
	return [to_arr_str](int k, const sffs_state& new_state, const sffs_state& old_state) {
		if (true) {
			std::cout << "k: " << k << ": " << to_string(new_state, to_arr_str) << " vs " << old_state.score;
			std::cout << "\n";
		}
		else if (old_state.data.size() == 0) {
			std::cout << "." << std::flush;
		}
	};
}

}
