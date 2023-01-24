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
		std::cout << "k: " << k << ": " << to_string(new_state, to_arr_str) << " vs " << old_state.score;
		std::cout << "\n";
	};
}

template <typename T>
double sffs_fitness_test(const test_setup<T>& ts) {
	auto cb = [](const test_battery_result& br, bool) {
		const auto analysis = get_worst_statistic_analysis(br);
		if (!analysis) {
			return true;
		}
		return analysis->pass();
	};

	const auto r = evaluate_multi_pass<T>(cb, ts);

	std::vector<double> all_p_values;
	for (const auto& tr : r.results) {
		append(all_p_values, to_p_values(tr.second));
	}
	return (ts.stop_power_of_two - r.power_of_two()) + kolmogorov_smirnov_stats(all_p_values)->value;
}

template <typename T>
double sffs_fitness_test(const mixer<T>& mixer) {
	return sffs_fitness_test<T>(create_mixer_test_setup<T>(mixer));
}

template <typename T>
double sffs_fitness_test_as_prng(const mixer<T>& mixer) {
	const auto setup = create_prng_test_setup<T>([mixer](T seed) {
		return create_prng_from_mixer<T>(mixer, seed);
	});
	return sffs_fitness_test<T>(setup);
}


}
