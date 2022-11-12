#pragma once

#include "search_mixer_constants.h"

namespace mixer {

inline config get_mx3_config() {
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
	const auto fitness = [factory](const bit_vector& bits) {
		const auto m = mixer{"mx3", factory(bits)};
		return sffs_fitness_test(m);
	};
	return {bits, fitness, to_str, to_arr_str};
}

inline config get_xmx_config() {
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
	const auto fitness = [factory](const bit_vector& bits) {
		const auto m = mixer{"xmx", factory(bits)};
		return sffs_fitness_test(m);
	};
	return {bits, fitness, to_str, to_arr_str};
}

inline void run_search() {
	auto cfg = get_xmx_config();
	cfg.seed = find_seed(cfg);
	start_search("xmx", cfg);
}

}
