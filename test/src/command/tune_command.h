#pragma once
#include "search/search_setup.h"

namespace mixer {

template <typename T>
double tune_fitness(T c) {
	constexpr int max_power = 27;
	const auto cb = [max_power](const test_battery_result& br) {
		const auto meta = get_meta_analysis(br);
		if (!meta) {
			return true;
		}
		return meta->pass() && br.power_of_two() < max_power;
	};

	int total = 0;
	int max = 0;
	for (T c1 = 16; c1 <= 16; ++c1) {
		int sum = 0;
		for (T c2 = 13; c2 <= 19; ++c2) {
			for (T c3 = 13; c3 <= 19; ++c3) {
				const search32::xmxmx_constants constants{c1, c2, c3, c, c};
				const auto mixer = create_xmxmx_mixer(constants);
				const test_setup<T> ts{
					mixer.name,
					create_rrc_sources<T>(),
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

	const auto fitness = [](const bit_vector& bits, unsigned int num_threads) {
		const auto c = bits.get<uint32_t>(0, 32);
		return tune_fitness(c);
	};

	bit_vector seed;
	seed.add(3016919661, 32);
	sffs_config cfg{32, fitness, to_str, to_str, seed, 10, 20};
	start_search<uint32_t>("TUNE", cfg);
}

}
