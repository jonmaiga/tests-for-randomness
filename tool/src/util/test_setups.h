#pragma once

#include "mixers64.h"
#include "prng.h"
#include "util/stream_sources.h"

namespace tfr {
template <typename T>
std::vector<seed_data> generate_seeds(int n) {
	std::set<T> added;
	for (const auto& seed : get_rrc_permutations<T>(1)) {
		added.insert(seed);
	}
	T x = 1234;
	const auto mix = get_default_mixer<T>();
	while (added.size() < n) {
		added.insert(mix(x++));
	}

	std::vector<seed_data> results;
	uint64_t y = 12345;
	for (auto primary : added) {
		seed_data s = {
			static_cast<uint64_t>(primary),
			mix64::mx3(y++),
			mix64::mx3(y++),
			mix64::mx3(y++)
		};
		results.push_back(s);
	}
	return results;
}

template <typename T>
streams<T> create_evenly_seeded_stream(const std::string& name, const std::vector<T>& data, int sample_count) {
	streams<T> ts;
	const uint64_t interval = data.size() / sample_count;
	std::cout << "Warning: " << name << " data samples will overlap for >2^" << std::floor(std::log2(interval)) << ", this will make meta analysis unreliable.\n";
	std::cout << "Warning: " << name << " data will rotate for >2^" << std::floor(std::log2(data.size())) << ", this will make sample results unreliable.\n";
	for (uint64_t i = 0; i < sample_count; ++i) {
		const auto start_index = i * interval;
		ts.push_back(
			create_stream_from_data_by_ref<T>(name + "-" + std::to_string(start_index), data, start_index)
		);
	}
	return ts;
}

template <typename T>
test_setup<T> create_data_test_setup(const std::string& name, const std::vector<T>& data, int sample_count = 128) {
	return test_setup<T>{
		name,
		create_evenly_seeded_stream<T>(name, data, sample_count),
		default_test_types
	};
}

template <typename T>
test_setup<T> create_combiner_test_setup(combiner<T> combiner) {
	return test_setup<T>{
		combiner.name,
		create_combiner_sources<T>(combiner),
		default_test_types
	};
}

template <typename T>
test_setup<T> create_mixer_test_setup(const mixer<T> mixer) {
	return test_setup<T>{
		mixer.name,
		create_sources<T>(),
		default_test_types,
		mixer,
	};
}

template <typename T>
test_setup<T> create_prng_test_setup(std::function<stream<T>(const seed_data& seed)> create_prng) {
	std::string test_subject_name;
	streams<T> to_test;
	for (auto seed : generate_seeds<T>(4 * bit_sizeof<T>())) {
		auto& prng = to_test.emplace_back(create_prng(seed));
		if (test_subject_name.empty()) {
			test_subject_name = prng.name;
		}
		prng.name += " seed=" + to_string(seed);
	}

	return test_setup<T>{
		test_subject_name,
		to_test,
		default_test_types,
	};
}
}