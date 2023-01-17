#pragma once

#include "util/stream_sources.h"

namespace mixer {

template <typename T>
std::vector<T> generate_seeds(int n) {
	std::set<T> seeds;
	for (const auto& seed : get_rrc_permutations<T>(1)) {
		seeds.insert(seed);
	}
	uint32_t x = 1234;
	const auto mix = get_default_mixer<T>();
	while (seeds.size() < n) {
		seeds.insert(mix(x++));
	}
	return {seeds.begin(), seeds.end()};
}

template <typename T>
streams<T> create_seeded_trng(int sample_count) {
	streams<T> ts;
	const auto& data = get_trng_data<T>();
	const uint64_t interval = data.size() / sample_count;
	for (uint64_t i = 0; i < sample_count; ++i) {
		const auto start_index = i * interval;
		ts.push_back(
			create_stream_from_data_by_ref<T>("trng-" + std::to_string(start_index), data, start_index)
		);
	}
	return ts;
}

template <typename T>
test_setup<T> create_trng_test_setup() {
	return test_setup<T>{
		"trng",
		create_seeded_trng<T>(128),
		all_test_types
	};
}

template <typename T>
test_setup<T> create_combiner_test_setup(combiner<T> combiner) {
	return test_setup<T>{
		combiner.name,
		create_combiner_sources<T>(combiner),
		all_test_types
	};
}

template <typename T>
test_setup<T> create_mixer_test_setup(const mixer<T> mixer) {
	return test_setup<T>{
		mixer.name,
		create_sources<T>(),
		all_test_types,
		mixer,
	};
}

template <typename T>
test_setup<T> create_prng_test_setup(std::function<stream<T>(T seed)> create_prng) {
	streams<T> to_test;
	for (auto seed : generate_seeds<T>(4 * bit_sizeof<T>())) {
		to_test.push_back(create_prng(seed));
	}

	return test_setup<T>{
		to_test.front().name,
		to_test,
		all_test_types,
	};
}


}
