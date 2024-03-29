#pragma once

#include <map>

#include "sffs_types.h"
#include "util/bitvector.h"

namespace tfr {

struct sffs_state {
	bit_vector data;
	double score = std::numeric_limits<double>::max();
};

using sffs_callback = std::function<void(int k, const sffs_state& new_state, const sffs_state& old_state)>;

inline sffs_state get_state(const sffs_config& config, const sffs_state& current_state, bool is_forward) {
	const auto& current_data = current_state.data;
	jobs<sffs_state> sffs_jobs;
	for (int i = 0; i < config.bits; ++i) {
		if (current_data.get_bit(i) == is_forward) continue;
		auto test = current_data;
		test.set_bit(i, is_forward);
		sffs_jobs.emplace_back([test, &config]() {
				const double score = config.fitness(test);
				assertion(is_valid(score), "invalid fitness score for sffs");
				return sffs_state{test, score};
			}
		);
	}

	sffs_state best;
	const auto collector = [&best](const sffs_state& result) {
		static std::mutex m;
		std::lock_guard lg(m);
		if (result.score < best.score) {
			best = result;
		}
	};
	run_jobs<sffs_state>(sffs_jobs, collector, default_max_threads());
	return best;
}

inline sffs_state get_forward_state(const sffs_config& config, const sffs_state& current_state) {
	return get_state(config, current_state, true);
}

inline sffs_state get_backward_state(const sffs_config& config, const sffs_state& current_state) {
	return get_state(config, current_state, false);
}

inline sffs_state run_sffs(const sffs_config& config, const sffs_callback& callback) {
	std::map<int, sffs_state> ks;
	bit_vector init_bits(config.bits);
	if (const auto& seed = config.seed) {
		init_bits = *seed;
	}
	const int min = config.min;
	const int max = std::min(config.bits, config.max);

	const double seed_score = config.fitness(init_bits);
	ks[init_bits.count()] = {init_bits, seed_score};
	int k = init_bits.count();

	while (k < max) {
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
