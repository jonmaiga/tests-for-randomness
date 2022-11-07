#pragma once

#include <eval_mixer.h>

#include "format_result.h"
#include "util/random.h"
#include "util/sffs.h"

namespace mixer {

static thread_local random rnd;

inline double sffs_fitness_test(const mixer& mixer) {
	const auto r = test_rrc(mixer, 100);
	auto pvs = to_p_values(r[s_type::sac]);
	append(pvs, to_p_values(r[s_type::chi2]));
	const auto pv = fishers_combined_probabilities(pvs);
	return 2 * std::abs(pv - 0.5);
}

inline config get_mx3_config() {
	struct constants {
		explicit constants(const bit_vector& bits) {
			C1 = bits.get(0, 6);
			C2 = bits.get(6, 6);
			C3 = bits.get(12, 6);
			C4 = bits.get(18, 6);
			m1 = bits.get(24, 64);
			m2 = bits.get(24 + 64, 64);
			m3 = bits.get(24 + 64 + 64, 64);
		}

		uint64_t C1;
		uint64_t C2;
		uint64_t C3;
		uint64_t C4;
		uint64_t m1;
		uint64_t m2;
		uint64_t m3;
	};

	auto factory = [](const bit_vector& bits) {
		return [c=constants(bits)](uint64_t x) {
			constexpr uint64_t C = 0xbea225f9eb34556d;
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

	auto to_str = [factory](const bit_vector& bits) {
		constexpr uint64_t C = 0xbea225f9eb34556d;
		const constants c(bits);
		std::stringstream ss;
		ss << "    x ^= x >> " << c.C1 << ";\n";
		ss << "    x *= " << c.m1 << "ull;\n";
		ss << "    x ^= x >> " << c.C2 << ";\n";
		ss << "    x *= " << c.m2 << "ull;\n";
		ss << "    x ^= x >> " << c.C3 << ";\n";
		ss << "    x *= " << c.m3 << "ull;\n";
		ss << "    x ^= x >> " << c.C4 << ";\n";
		ss << "tune\n"; // << evaluate_1d(rnd, factory(bits), 10000000) << "\n";
		return ss.str();
	};

	auto to_arr_str = [](const bit_vector& bits) {
		constants c(bits);
		std::stringstream ss;
		ss << c.C1 << ", " << c.C2 << ", " << c.C3 << ", " << c.C4;
		return ss.str();
	};

	constexpr int bits = 24 + 64 + 64 + 64;
	const auto fitness = [factory](const bit_vector& bits) {
		const auto m = mixer{"mx3", factory(bits)};
		return sffs_fitness_test(m);
	};
	auto seed = bit_vector(bits);
	seed.set(31, 0, 6);
	seed.set(32, 6, 6);
	seed.set(29, 12, 6);
	seed.set(33, 18, 6);
	seed.set(0xbea225f9eb34556d, 24, 64);
	seed.set(0xe9846af9b1a615dull, 98, 64);
	seed.set(0x9E6C63D0676A9A99ull, 98 + 64, 64);
	return {bits, seed, fitness, to_str, to_arr_str};
}

inline void start_tune(const std::string& name, const config& config) {
	std::cout << "===========================\n";
	std::cout << name << " " << config.bits << " bits\n";
	std::cout << "===========================\n";
	std::cout << "Baseline fitness: " << sffs_fitness_test(mx3) << "\n";
	const auto result = run_sffs(config, true);
	std::stringstream trial_out;
	trial_out << to_string(result, config.to_arr_str) << "\n";
	trial_out << config.to_string(result.data);
	std::cout << trial_out.str();
}

inline void run_search() {
	start_tune("mx3", get_mx3_config());
}

}
