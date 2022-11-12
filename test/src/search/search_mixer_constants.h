#pragma once

#include <eval_parallel.h>

#include "format_result.h"
#include "util/random.h"
#include "util/sffsutils.h"

namespace mixer {

inline double sffs_fitness_test(const mixer& mixer) {
	const auto r = test_rrc(mixer, 5000);
	std::vector<double> all;
	for (const auto& tr : r.results) {
		append(all, to_p_values(tr.second));
	}
	return kolmogorov_smirnov_stats(all);
	const double pv = fishers_combined_probabilities(all);
	return 2 * std::abs(pv - 0.5);
}

inline bit_vector random_bit_vector(int bits) {
	static random r(1234);
	bit_vector bv(bits);
	for (int b = 0; b < bits; ++b) {
		bv.set_bit(b, r() & 1);
	}
	return bv;
}

inline bit_vector find_seed(const config& config) {
	double best_score = std::numeric_limits<double>::max();
	bit_vector best(config.bits);
	for (int i = 0; i < 30; ++i) {
		bit_vector r = random_bit_vector(config.bits);
		const double score = config.fitness(r);
		if (score < best_score) {
			best_score = score;
			best = r;
		}
	}
	return best;
}

inline void start_search(const std::string& name, const config& config) {
	const auto trng_stream = create_stream_from_data_by_ref_thread_safe("trng", get_trng_data());
	const auto trng = create_mixer_from_stream("trng1", trng_stream);

	std::cout << "===========================\n";
	std::cout << name << " " << config.bits << " bits\n";
	std::cout << "===========================\n";
	std::cout << config.to_string(config.seed) << "\n";
	std::cout << "Baseline fitness: " << config.fitness(config.seed) << "\n";
	std::cout << "Trng fitness    : " << sffs_fitness_test(trng) << "\n";
	const auto result = run_sffs(config, create_sffs_printer(config.to_arr_str));
	std::stringstream ss;
	ss << to_string(result, config.to_arr_str) << "\n";
	ss << config.to_string(result.data);
	std::cout << ss.str();
}

}
