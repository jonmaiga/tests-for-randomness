#pragma once

#include <iostream>
#include <map>
#include <sstream>

#include "bit_vector.h"
#include "sffs_types.h"

namespace mixer {

struct sffs_state {
	bit_vector data;
	double score = 100000000;
};

inline std::string to_string(const sffs_state& s, const bit_vector_to_string& to_arr_str) {
	std::stringstream ss;
	ss << "(" << s.data.count() << "): " << to_string(s.data);
	if (to_arr_str) {
		ss << " [  " << to_arr_str(s.data) << " ]";
	}
	ss << " : " << s.score;
	return ss.str();
}

inline sffs_state get_state(const config& config, const sffs_state& current_state, bool is_forward) {
	sffs_state best;
	for (int i = 0; i < config.bits; ++i) {
		if (current_state.data.get_bit(i) == is_forward) continue;
		auto test = current_state.data;
		test.set_bit(i, is_forward);

		const double score = config.fitness(test);
		if (score < best.score) {
			best = {test, score};
		}
	}
	return best;
}


inline sffs_state get_forward_state(const config& config, const sffs_state& current_state) {
	return get_state(config, current_state, true);
}

inline sffs_state get_backward_state(const config& config, const sffs_state& current_state) {
	return get_state(config, current_state, false);
}

inline sffs_state run_sffs(const config& config, bool verbose) {
	std::map<int, sffs_state> ks;
	const bit_vector init_bits(config.bits);
	//ks[0] = state{init_bits, config.f(init_bits)};
	double best_score = std::numeric_limits<double>::max();
	int min = 0;
	int max = config.bits;
	int k = 0;
	if (config.seed.size() != 0) {
		const double seed_score = config.fitness(config.seed);
		best_score = seed_score;
		ks[config.seed.count()] = {config.seed, seed_score};
		k = config.seed.count();
		std::cout << "seed@" << k << "        " << to_string(config.seed) << " (" << seed_score << ")\n";
	}

	while (k < max) {
		{
			const sffs_state new_fwd_state = get_forward_state(config, ks[k]);
			const sffs_state old_fwd_state = ks[k + 1];
			if (old_fwd_state.data.size() == 0 || new_fwd_state.score < old_fwd_state.score) {
				ks[k + 1] = new_fwd_state;
			}
			++k;
			if (verbose) {
				std::cout << "fwd: " << k << ": " << to_string(new_fwd_state, config.to_arr_str) << " vs " << old_fwd_state.score;
				if (new_fwd_state.score < best_score) {
					std::cout << "*";
					best_score = new_fwd_state.score;
				}
				std::cout << "\n";
			}
			else if (old_fwd_state.data.size() == 0) {
				std::cout << "." << std::flush;
			}
		}

		while (k > min) {
			const sffs_state new_bwd_state = get_backward_state(config, ks[k]);
			const sffs_state old_bwd_state = ks[k - 1];
			if (old_bwd_state.data.size() == 0 || new_bwd_state.score < old_bwd_state.score) {
				ks[k - 1] = new_bwd_state;
				--k;
				if (verbose) {
					std::cout << "bwd: " << k << ": " << to_string(new_bwd_state, config.to_arr_str) << " < " << old_bwd_state.score;
					if (new_bwd_state.score < best_score) {
						std::cout << "*";
						best_score = new_bwd_state.score;
					}
					std::cout << "\n";
				}
			}
			else break;
		}
	}

	if (!verbose) {
		std::cout << "\n";
	}

	sffs_state best;
	for (const auto& e : ks) {
		if (e.second.score < best.score) {
			best = e.second;
		}
	}
	return best;
}


}
