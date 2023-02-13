#pragma once

#include "search_configs.h"
#include "util/random.h"
#include "util/sffs.h"
#include "util/sffsutils.h"

namespace tfr {


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
		const double score = config.fitness(r);
		if (score < best_score) {
			best_score = score;
			best = r;
		}
	}
	return best;
}

template <typename T>
sffs_state start_search(const std::string& name, const sffs_config& config) {
	//auto trng_stream = create_stream_from_data_by_ref_thread_safe<T>("trng", get_trng_data<T>());
	//const auto trng = create_mixer_from_stream("trng1", trng_stream);

	std::cout << "===========================\n";
	std::cout << name << " " << config.bits << " bits\n";
	std::cout << "===========================\n";
	if (const auto& seed = config.seed) {
		if (seed->size() != config.bits) {
			std::cout << "Seed size is not correct, expected " << config.bits << " but seed is " << seed->size() << ".\n";
			return {};
		}
		std::cout << config.to_string(*seed) << "\n";
		std::cout << "Seed fitness: " << config.fitness(*seed) << "\n";
	}
	//std::cout << "Trng fitness    : " << sffs_fitness_test(trng, default_max_threads()) << "\n";
	auto result = run_sffs(config, create_sffs_printer(config.to_arr_str));
	std::stringstream ss;
	ss << to_string(result, config.to_arr_str) << "\n";
	ss << config.to_string(result.data);
	std::cout << ss.str();
	return result;
}

template <typename T>
void run_sffs() {
	bit_vector seed;
	seed.add(16, 5);
	seed.add(15, 5);
	seed.add(14, 5);
	seed.add(19, 5);
	seed.add(7, 5);

	auto cfg = get_xm2x_config<T>();
	//cfg.seed = seed;
	cfg.seed = find_seed(cfg, 100);

	const auto result = start_search<T>("NAME HERE", cfg);

	const auto mixer = create_xm2x_mixer<T>(result.data);
	evaluate_multi_pass(create_result_callback(false), create_mixer_test_setup<T>(mixer));
}

}
