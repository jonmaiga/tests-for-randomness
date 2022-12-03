#pragma once

#include <eval_parallel.h>

#include "format_result.h"
#include "../util/sffsutils.h"
#include "util/random.h"

namespace mixer {

template <typename T>
double sffs_fitness_test(const mixer<T>& mixer, unsigned int threads) {
	int max_power = 15;
	uint64_t n = 1ull << max_power;
	auto ts = test_setup<T>{
		mixer.name,
		create_rrc_sources<T>(),
		all_test_types,
		mixer,
		threads
	};

	auto cb = [max_power](const test_battery_result& br) {
		const auto meta = get_meta_analysis(br);
		if (!meta) {
			return true;
		}
		return meta->pass() && br.power_of_two() < max_power;
	};

	const auto r = test_parallel_multi_pass<T>(cb, ts);
	
	std::vector<double> all;
	for (const auto& tr : r.results) {
		append(all, to_p_values(tr.second));
	}
	return (max_power - r.power_of_two()) + kolmogorov_smirnov_stats(all)->value;
	//return kolmogorov_smirnov_stats(all)->value;
}

inline bit_vector random_bit_vector(int bits) {
	static random r(1234);
	bit_vector bv(bits);
	for (int b = 0; b < bits; ++b) {
		bv.set_bit(b, r() & 1);
	}
	return bv;
}

inline bit_vector find_seed(const sffs_config& config, int tries) {
	double best_score = std::numeric_limits<double>::max();
	bit_vector best(config.bits);
	for (int i = 0; i < tries; ++i) {
		bit_vector r = random_bit_vector(config.bits);
		const double score = config.fitness(r, default_max_threads());
		if (score < best_score) {
			best_score = score;
			best = r;
		}
	}
	return best;
}

template <typename T>
sffs_state start_search(const std::string& name, const sffs_config& config) {
	auto trng_stream = create_stream_from_data_by_ref_thread_safe<T>("trng", get_trng_data<T>());
	const auto trng = create_mixer_from_stream("trng1", trng_stream);

	std::cout << "===========================\n";
	std::cout << name << " " << config.bits << " bits\n";
	std::cout << "===========================\n";
	if (const auto& seed = config.seed) {
		std::cout << config.to_string(*seed) << "\n";
		std::cout << "Baseline fitness: " << config.fitness(*seed, default_max_threads()) << "\n";
	}
	std::cout << "Trng fitness    : " << sffs_fitness_test(trng, default_max_threads()) << "\n";
	auto result = run_sffs(config, create_sffs_printer(config.to_arr_str));
	std::stringstream ss;
	ss << to_string(result, config.to_arr_str) << "\n";
	ss << config.to_string(result.data);
	std::cout << ss.str();
	return result;
}

}
