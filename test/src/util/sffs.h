#pragma once

#include <map>

#include "bit_vector.h"
#include "sffs_types.h"

namespace mixer {

struct sffs_state {
	bit_vector data;
	double score = 100000000;
};

using sffs_callback = std::function<void(int k, const sffs_state& new_state, const sffs_state& old_state)>;

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

inline sffs_state run_sffs(const config& config, const sffs_callback& callback) {
	std::map<int, sffs_state> ks;
	const bit_vector init_bits(config.bits);
	const int min = 0;
	const int max = config.bits;
	int k = 0;
	if (config.seed.size() != 0) {
		const double seed_score = config.fitness(config.seed);
		ks[config.seed.count()] = {config.seed, seed_score};
		k = config.seed.count();
	}

	while (k < max) {
		{
			const sffs_state new_fwd_state = get_forward_state(config, ks[k]);
			const sffs_state old_fwd_state = ks[k + 1];
			if (old_fwd_state.data.size() == 0 || new_fwd_state.score < old_fwd_state.score) {
				ks[k + 1] = new_fwd_state;
			}
			++k;
			if (callback) {
				callback(k, new_fwd_state, old_fwd_state);
			}
		}

		while (k > min) {
			const sffs_state new_bwd_state = get_backward_state(config, ks[k]);
			const sffs_state old_bwd_state = ks[k - 1];
			if (old_bwd_state.data.size() == 0 || new_bwd_state.score < old_bwd_state.score) {
				ks[k - 1] = new_bwd_state;
				--k;
				if (callback) {
					callback(k, new_bwd_state, old_bwd_state);
				}
			}
			else break;
		}
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
