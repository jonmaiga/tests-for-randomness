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

}
