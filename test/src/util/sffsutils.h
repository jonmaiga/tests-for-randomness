#pragma once

#include <iostream>
#include <sstream>

#include "sffs.h"

namespace mixer {

inline std::string to_string(const sffs_state& s, const bit_vector_to_string& to_arr_str) {
	std::stringstream ss;
	ss << to_string(s.data);
	if (to_arr_str) {
		ss << " [ " << to_arr_str(s.data) << " ]";
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

template <typename T>
double sffs_fitness_test(const mixer<T>& mixer, unsigned int threads) {

	auto ts = test_setup<T>{
		mixer.name,
		create_rrc_sources<T>(),
		all_test_types,
		mixer,
		threads
	};

	constexpr int max_power = 25;
	auto cb = [max_power](const test_battery_result& br) {
		const auto meta = get_meta_analysis(br);
		if (!meta) {
			return true;
		}
		return meta->pass() && br.power_of_two() < max_power;
	};

	const auto r = test_parallel_multi_pass<T>(cb, ts);

	std::vector<double> all_p_values;
	for (const auto& tr : r.results) {
		append(all_p_values, to_p_values(tr.second));
	}
	return (max_power - r.power_of_two()) + kolmogorov_smirnov_stats(all_p_values)->value;
	//return kolmogorov_smirnov_stats(all_p_values)->value;
}


}
