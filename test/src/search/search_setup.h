#pragma once

#include "search_mixer_constants.h"
#include "search_configs32.h"

namespace mixer {

inline sffs_config get_mx3_config() {
	struct constants {
		explicit constants(const bit_vector& bits) {
			//m1 = bits.get(0, 64);
			C1 = bits.get(0, 6);
			C2 = bits.get(6, 6);
			C3 = bits.get(12, 6);
			C4 = bits.get(18, 6);
			//m1 = bits.get(24, 64);
			//m2 = bits.get(24 + 64, 64);
			//m3 = bits.get(24 + 64 + 64, 64);
		}

		uint64_t C1 = 32;
		uint64_t C2 = 29;
		uint64_t C3 = 32;
		uint64_t C4 = 29;
		//uint64_t m1;
		//uint64_t m2;
		//uint64_t m3;
	};

	auto factory = [](const bit_vector& bits) {
		return [c=constants(bits)](uint64_t x) {
			const uint64_t C = 0xbea225f9eb34556d;
			x ^= (x >> c.C1);
			x *= C;
			x ^= (x >> c.C2);
			x *= C;
			x ^= (x >> c.C3);
			x *= C;
			x ^= (x >> c.C4);
			return x;
		};
	};

	auto to_str = [](const bit_vector& bits) {

		const constants c(bits);
		const uint64_t C = 0xbea225f9eb34556d;
		std::stringstream ss;
		ss << "    x ^= x >> " << c.C1 << ";\n";
		ss << "    x *= " << C << "ull;\n";
		ss << "    x ^= x >> " << c.C2 << ";\n";
		ss << "    x *= " << C << "ull;\n";
		ss << "    x ^= x >> " << c.C3 << ";\n";
		ss << "    x *= " << C << "ull;\n";
		ss << "    x ^= x >> " << c.C4 << ";\n";
		return ss.str();
	};

	auto to_arr_str = [](const bit_vector& bits) {
		const constants c(bits);
		std::stringstream ss;
		ss << c.C1 << ", " << c.C2 << ", " << c.C3 << ", " << c.C4;
		return ss.str();
	};

	constexpr int bits = 24; // + 64; // + 64 + 64; // + 64; // + 64 + 64 + 64;
	const auto fitness = [factory](const bit_vector& bits, unsigned int num_threads) {
		const auto m = mixer64{"mx3", factory(bits)};
		return sffs_fitness_test(m, num_threads);
	};
	return {bits, fitness, to_str, to_arr_str};
}

inline sffs_config get_xmx_config() {
	struct constants {
		explicit constants(const bit_vector& bits) {
			C1 = bits.get(0, 6);
			C2 = bits.get(6, 6);
			m1 = bits.get(12, 64);
		}

		uint64_t C1;
		uint64_t C2;
		uint64_t m1;
	};

	auto factory = [](const bit_vector& bits) {
		return [c=constants(bits)](uint64_t x) {
			x ^= (x >> c.C1);
			x *= c.m1;
			x ^= (x >> c.C2);
			return x;
		};
	};

	auto to_str = [](const bit_vector& bits) {
		const constants c(bits);
		std::stringstream ss;
		ss << "    x ^= x >> " << c.C1 << ";\n";
		ss << "    x *= " << c.m1 << "ull;\n";
		ss << "    x ^= x >> " << c.C2 << ";\n";
		return ss.str();
	};

	auto to_arr_str = [](const bit_vector& bits) {
		const constants c(bits);
		std::stringstream ss;
		ss << c.C1 << ", " << c.C2 << ", " << c.m1;
		return ss.str();
	};

	constexpr int bits = 12 + 64;
	const auto fitness = [factory](const bit_vector& bits, unsigned int num_threads) {
		const auto m = mixer64{"xmx", factory(bits)};
		return sffs_fitness_test(m, num_threads);
	};
	return {bits, fitness, to_str, to_arr_str};
}

template <typename T> sffs_config get_xmx_config() {
	if constexpr (sizeof(T) == 4) {
		return search32::get_xmx_config();
	}
	else {
		return {};
	}
}


template <typename T> sffs_config get_xmxmx_config() {
	if constexpr (sizeof(T) == 4) {
		return search32::get_xmxmx_config();
	}
	else {
		return {};
	}
}

template <typename T> sffs_config get_xmxmxmx_config() {
	if constexpr (sizeof(T) == 4) {
		return search32::get_xmxmxmx_config();
	}
	else {
		return {};
	}
}


template <typename T>
void run_search() {
	auto cfg = get_xmxmx_config<T>();
	bit_vector seed(47);
	seed.set(16, 0, 5);
	seed.set(16, 5, 5);
	seed.set(16, 10, 5);
	seed.set(0x9b1a615dull, 10, 32);
	cfg.seed = seed;
	//cfg.seed = find_seed(cfg, 10000);
	const auto result = start_search<T>("NAME HERE", cfg);
	const auto mixer = search32::create_xmxmx_mixer(result.data);
	test_parallel_multi_pass(create_result_callback(25, false), create_test_setup<T>(mixer));
}

}
