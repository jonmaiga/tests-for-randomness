#pragma once

#include <eval_parallel.h>

#include "format_result.h"
#include "util/random.h"
#include "util/sffsutils.h"

namespace mixer {

static thread_local random rnd;

inline double sffs_fitness_test(const mixer& mixer) {
	const auto r = test_rrc(mixer, 550);
	//  std::vector<double> pvs;
	//  for (const auto& tr : r.results) {
	//  	append(pvs, to_p_values(tr.second));
	// }
	// auto bs = basic_stats(pvs);
	// return count_fails(pvs, 0.005);

	std::vector<double> all;
	for (const auto& tr : r.results) {
		all.push_back(fishers_combined_probabilities(normalize_to_uniform(to_p_values(tr.second))));
	}
	const double pv = fishers_combined_probabilities(all);
	return 2 * std::abs(pv - 0.5);
}

inline config get_mx3_config() {
	struct constants {
		explicit constants(const bit_vector& bits) {
			//m1 = bits.get(0, 64);
			C1 = bits.get(0, 6);
			C2 = bits.get(6, 6);
			C3 = bits.get(12, 6);
			C4 = bits.get(18, 6);
			m1 = bits.get(24, 64);
			m2 = bits.get(24 + 64, 64);
			m3 = bits.get(24 + 64 + 64, 64);
		}

		uint64_t C1 = 32;
		uint64_t C2 = 29;
		uint64_t C3 = 32;
		uint64_t C4 = 29;
		uint64_t m1;
		uint64_t m2;
		uint64_t m3;
	};

	auto factory = [](const bit_vector& bits) {
		return [c=constants(bits)](uint64_t x) {
			const uint64_t C = c.m1; //0xbea225f9eb34556d;
			x ^= (x >> c.C1);
			x *= c.m1;
			x ^= (x >> c.C2);
			x *= c.m2;
			x ^= (x >> c.C3);
			x *= c.m3;
			x ^= (x >> c.C4);
			return x;
		};
	};

	auto to_str = [](const bit_vector& bits) {

		const constants c(bits);
		const uint64_t C = c.m1; //0xbea225f9eb34556d;
		std::stringstream ss;
		ss << "    x ^= x >> " << c.C1 << ";\n";
		ss << "    x *= " << c.m1 << "ull;\n";
		ss << "    x ^= x >> " << c.C2 << ";\n";
		ss << "    x *= " << c.m2 << "ull;\n";
		ss << "    x ^= x >> " << c.C3 << ";\n";
		ss << "    x *= " << c.m3 << "ull;\n";
		ss << "    x ^= x >> " << c.C4 << ";\n";
		return ss.str();
	};

	auto to_arr_str = [](const bit_vector& bits) {
		const constants c(bits);
		std::stringstream ss;
		ss << c.C1 << ", " << c.C2 << ", " << c.C3 << ", " << c.C4;
		return ss.str();
	};

	constexpr int bits = 24 + 64 + 64 + 64; // + 64; // + 64 + 64 + 64;
	const auto fitness = [factory](const bit_vector& bits) {
		const auto m = mixer{"mx3", factory(bits)};
		return sffs_fitness_test(m);
	};
	auto seed = bit_vector(bits);
	seed.set(37, 0, 6);
	seed.set(59, 6, 6);
	seed.set(33, 12, 6);
	seed.set(31, 18, 6);
	seed.set(16139256160673849215ull, 24, 64);
	seed.set(18373938741426217439ull, 24 + 64, 64);
	seed.set(16125138431304736415ull, 24 + 64 + 64, 64);
	return {bits, seed, fitness, to_str, to_arr_str};
}

inline void start_tune(const std::string& name, const config& config) {
	std::cout << "===========================\n";
	std::cout << name << " " << config.bits << " bits\n";
	std::cout << "===========================\n";
	std::cout << config.to_string(config.seed) << "\n";
	std::cout << "Baseline fitness: " << config.fitness(config.seed) << "\n";
	const auto result = run_sffs(config, create_sffs_printer(config.to_arr_str));
	std::stringstream ss;
	ss << to_string(result, config.to_arr_str) << "\n";
	ss << config.to_string(result.data);
	std::cout << ss.str();
}

inline void run_search() {
	start_tune("mx3", get_mx3_config());
}

}
