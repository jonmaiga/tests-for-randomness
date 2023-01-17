#pragma once
#include "search/search_setup.h"

namespace mixer {

template <typename T>
double tune_fitness(T c) {
	constexpr int max_power = 27;
	const auto cb = [max_power](const test_battery_result& br) {
		const auto analysis = get_worst_statistic_analysis(br);
		if (!analysis) {
			return true;
		}
		return analysis->pass() && br.power_of_two() < max_power;
	};

	int total = 0;
	int max = 0;
	for (T c1 = 16; c1 <= 16; ++c1) {
		int sum = 0;
		for (T c2 = 13; c2 <= 16; ++c2) {
			for (T c3 = 13; c3 <= 16; ++c3) {
				const xm2x_constants<T> constants{c1, c2, c3, c};
				const auto mixer = create_xm2x_mixer(constants);
				const test_setup<T> ts{
					mixer.name,
					create_sources<T>(),
					all_test_types,
					mixer,
					default_max_threads()
				};

				const auto r = evaluate_multi_pass<T>(cb, ts);
				sum += r.power_of_two();
				max += max_power;
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
