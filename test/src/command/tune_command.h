#pragma once
#include "search/search_setup.h"

namespace mixer {

template <typename T>
double tune_fitness(T c) {
	const auto cb = [](const test_battery_result& br, bool) {
		const auto analysis = get_worst_statistic_analysis(br);
		if (!analysis) {
			return true;
		}
		return analysis->pass();
	};

	int total = 0;
	int max = 0;
	for (T c1 = 16; c1 <= 16; ++c1) {
		int sum = 0;
		for (T c2 = 13; c2 <= 16; ++c2) {
			for (T c3 = 13; c3 <= 16; ++c3) {
				const xm2x_constants<T> constants{c1, c2, c3, c};
				const auto mixer = create_xm2x_mixer(constants);
				const auto setup = create_mixer_test_setup<T>(mixer);
				const auto r = evaluate_multi_pass<T>(cb, setup);
				sum += r.power_of_two();
				max += setup.stop_power_of_two;
			}
		}
		total += sum;
	}
	std::cout << ".";
	return max - total;
}


inline void tune_command() {
	auto to_str = [](const bit_vector& bits) {
		return std::to_string(bits.get(0, 32));
	};

	const auto fitness = [](const bit_vector& bits) {
		const auto c = bits.get<uint32_t>(0, 32);
		return tune_fitness(c);
	};

	bit_vector seed;
	seed.add(3016919661, 32);
	const sffs_config cfg{32, fitness, to_str, to_str, seed, 10, 20};
	start_search<uint32_t>("TUNE", cfg);
}

}
