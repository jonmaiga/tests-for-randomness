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
	T x = static_cast<T>(1234);
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

template <typename RangeT, typename T = typename RangeT::value_type>
streams<T> create_evenly_seeded_stream(const std::string& name, const RangeT& data, int sample_count) {
	streams<T> ts;
	const uint64_t interval = data.size() / sample_count;
	std::cout << "Warning: " << name << " data samples will overlap for >2^" << std::floor(std::log2(interval)) << ", this will make meta analysis unreliable.\n";
	std::cout << "Warning: " << name << " data will rotate for >2^" << std::floor(std::log2(data.size())) << ", this will make sample results unreliable.\n";
	for (uint64_t i = 0; i < sample_count; ++i) {
		const auto start_index = i * interval;
		ts.push_back(
			create_stream_from_data(name + "-" + std::to_string(start_index), data, start_index)
		);
	}
	return ts;
}

template <typename RangeT, typename T = typename RangeT::value_type>
test_setup<T> create_data_test_setup(const std::string& name, const RangeT& data, int sample_count = 128) {
	return test_setup<T>{
		name,
		create_evenly_seeded_stream(name, data, sample_count),
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
test_setup<T> create_prng_test_setup(prng_factory<T> create_prng) {
	const auto max_unique_seeds = static_cast<uint64_t>(std::numeric_limits<T>::max() - 1);
	auto seeds = generate_seeds<T>(std::min(max_unique_seeds, static_cast<uint64_t>(1024)));
	const auto pop_seed = [&seeds]() {
		assertion(!seeds.empty(), "Out of seeds");
		const auto seed = seeds.back();
		seeds.pop_back();
		return seed;
	};

	streams<T> to_test;
	const auto add = [&to_test, create_prng](const seed_data& seed, std::function<stream<T>(stream<T>)> outer = {}) {
		assertion(seed.s64() != 0, "Avoid zero seeds");
		auto& prng = to_test.emplace_back(outer ? outer(create_prng(seed)) : create_prng(seed));
		prng.name += " seed=" + to_string(seed);
	};

	for (int bit = 1; bit < bit_sizeof<T>(); ++bit) {
		add(pop_seed(), [bit](stream<T> s) { return create_ror_stream<T>(s, bit); });
	}

	while (to_test.size() < 4 * bit_sizeof<T>()) {
		add(pop_seed());
	}

	return test_setup<T>{
		create_prng({0, 0, 0, 0}).name,
		to_test,
		default_test_types,
	};
}
}
