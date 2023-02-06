#pragma once

#include <iostream>
#include <sstream>

#include "sffs.h"

namespace tfr {

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

inline double get_score(const test_battery_result& result, int max_power_of_two) {
	const double s1 = max_power_of_two - result.power_of_two();

	std::vector<double> all_p_values;
	for (const auto& tr : result.results) {
		append(all_p_values, to_p_values(tr.second));
	}

	const double s2 = kolmogorov_smirnov_stats(all_p_values)->value;
	double s3 = 0;
	for (const auto& analysis : get_analysis(result)) {
		s3 = create_uniform_p_values_analysis(analysis.results).stat.value;
		break;
	}

	return s1 + s2 + s3;
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
	return get_score(r, ts.stop_power_of_two);
}

template <typename T>
double sffs_fitness_test(const mixer<T>& mixer) {
	return sffs_fitness_test<T>(create_mixer_test_setup<T>(mixer).range(10, 25));
}

template <typename T>
double sffs_fitness_test_as_prng(const mixer<T>& mixer) {
	const auto setup = create_prng_test_setup<T>([mixer](T seed) {
		return create_prng_from_mixer<T>(mixer, seed);
	});
	return sffs_fitness_test<T>(setup);
}


}
