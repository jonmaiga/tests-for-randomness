#pragma once

#include "search_configs32.h"
#include "search_configs64.h"
#include "util/random.h"
#include "util/sffs.h"
#include "util/sffsutils.h"

namespace mixer {


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
	//auto trng_stream = create_stream_from_data_by_ref_thread_safe<T>("trng", get_trng_data<T>());
	//const auto trng = create_mixer_from_stream("trng1", trng_stream);

	std::cout << "===========================\n";
	std::cout << name << " " << config.bits << " bits\n";
	std::cout << "===========================\n";
	if (const auto& seed = config.seed) {
		std::cout << config.to_string(*seed) << "\n";
		std::cout << "Seed fitness: " << config.fitness(*seed, default_max_threads()) << "\n";
	}
	//std::cout << "Trng fitness    : " << sffs_fitness_test(trng, default_max_threads()) << "\n";
	auto result = run_sffs(config, create_sffs_printer(config.to_arr_str));
	std::stringstream ss;
	ss << to_string(result, config.to_arr_str) << "\n";
	ss << config.to_string(result.data);
	std::cout << ss.str();
	return result;
}

template <typename T> sffs_config get_m_config() {
	if constexpr (sizeof(T) == 4) {
		return search32::get_m_config();
	}
	else {
		return {};
	}
}

template <typename T> sffs_config get_xmx_config() {
	if constexpr (sizeof(T) == 4) {
		return search32::get_xmx_config();
	}
	else {
		return {};
	}
}

template <typename T> sffs_config get_xmxmx_config() {
	if constexpr (sizeof(T) == 4) {
		return search32::get_xmxmx_config();
	}
	else {
		return search64::get_xmxmx_config();
	}
}

template <typename T> sffs_config get_xm3x_config() {
	if constexpr (sizeof(T) == 4) {
		return search32::get_xm3x_config();
	}
	else {
		return {};
	}
}


template <typename T>
void run_sffs() {

	bit_vector seed;
	seed.add(16, 5);
	seed.add(15, 5);
	seed.add(16, 5);
	seed.add(15, 5);
	//seed.add(0b01110101010101110101010101010101, 32);

	auto cfg = get_xm3x_config<T>();
	cfg.seed = seed;
	//cfg.seed = find_seed(cfg, 1000);
	const auto result = start_search<T>("NAME HERE", cfg);
	const auto c = search32::to_xm3x_constants(result.data);
	const auto mixer = search32::create_xm3x_mixer(c);
	evaluate_multi_pass(create_result_callback(25, false), create_test_setup<T>(mixer));
}

}
