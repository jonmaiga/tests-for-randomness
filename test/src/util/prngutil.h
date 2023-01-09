#pragma once

namespace mixer {

template <typename T>
std::vector<T> generate_seeds(int n) {
	std::set<T> seeds;
	for (const auto& seed : get_rrc_permutations<T>(1)) {
		seeds.insert(seed);
	}
	uint32_t x = 1234;
	while (seeds.size() < n) {
		seeds.insert(mix32::xm3x(x++));
	}
	return {seeds.begin(), seeds.end()};
}

template <typename T>
test_setup<T> create_prng_setup(std::function<stream<T>(T seed)> create_prng) {
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
