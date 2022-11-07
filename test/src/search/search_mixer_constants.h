#pragma once

#include "util/random.h"
#include "util/sffs.h"

namespace mixer {

static thread_local random rnd;

inline double sffs_fitness_test(const mixer& mixer) {
	const auto stream = create_counter_stream(1);
	const auto stat = avalanche_mixer_sac_test(100000, stream, mixer).front();
	return std::abs(stat.p_value - 0.5);
	//return stat.value;
}

inline config get_mx3_config() {
	struct constants {
		explicit constants(const bit_vector& bits) {
			C1 = bits.get(0, 6);
			C2 = bits.get(6, 6);
		}

		uint64_t C1;
		uint64_t C2;
	};

	auto factory = [](const bit_vector& bits) {
		return [c=constants(bits)](uint64_t x) {
			constexpr uint64_t C = 0xbea225f9eb34556d;
			x ^= (x >> c.C1);
			x *= C;
			x ^= (x >> c.C2);
			x *= C;
			x ^= (x >> c.C1);
			x *= C;
			x ^= (x >> c.C2);
			return x;
		};
	};

	auto to_str = [factory](const bit_vector& bits) {
		constexpr uint64_t C = 0xbea225f9eb34556d;
		const constants c(bits);
		std::stringstream ss;
		ss << "    x ^= x >> " << c.C1 << ";\n";
		ss << "    x *= " << C << "ull;\n";
		ss << "    x ^= x >> " << c.C2 << ";\n";
		ss << "    x *= " << C << "ull;\n";
		ss << "    x ^= x >> " << c.C1 << ";\n";
		ss << "    x *= " << C << "ull;\n";
		ss << "    x ^= x >> " << c.C2 << ";\n";
		ss << "tune\n"; // << evaluate_1d(rnd, factory(bits), 10000000) << "\n";
		return ss.str();
	};

	auto to_arr_str = [](const bit_vector& bits) {
		constants c(bits);
		std::stringstream ss;
		ss << c.C1 << ", " << c.C2;
		return ss.str();
	};

	constexpr int bits = 12;
	const auto fitness = [factory](const bit_vector& bits) {
		const auto m = mixer{"mx3", factory(bits)};
		return sffs_fitness_test(m);
	};
	return {bits, bit_vector(bits), fitness, to_str, to_arr_str};
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
