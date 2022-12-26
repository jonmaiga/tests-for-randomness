#pragma once

#include "util/bitvector.h"
#include "util/sffsutils.h"
#include "util/sffs_types.h"

namespace mixer {namespace search64 {

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


struct xm2x_constants {
	uint64_t C1;
	uint64_t C2;
	uint64_t C3;
	uint64_t m1;
};

inline xm2x_constants to_xm2x_constants(const bit_vector& bits) {
	using T = uint64_t;
	return {bits.get<T>(0, 6), bits.get<T>(6, 6), bits.get<T>(12, 6), bits.get<T>(18, 64)};
}

inline mixer64 create_xm2x_mixer(const xm2x_constants& c) {
	return mixer64{
		"xm2x", [c](uint64_t x) {
			x ^= (x >> c.C1);
			x *= c.m1;
			x ^= (x >> c.C2);
			x *= c.m1;
			x ^= (x >> c.C3);
			return x;
		}
	};
};


inline sffs_config get_xm2x_config() {
	auto to_str = [](const bit_vector& bits) {
		const xm2x_constants c = to_xm2x_constants(bits);
		std::stringstream ss;
		ss << "    x ^= x >> " << c.C1 << ";\n";
		ss << "    x *= " << c.m1 << "ull;\n";
		ss << "    x ^= x >> " << c.C2 << ";\n";
		ss << "    x *= " << c.m1 << "ull;\n";
		ss << "    x ^= x >> " << c.C3 << ";\n";
		return ss.str();
	};

	auto to_arr_str = [](const bit_vector& bits) {
		const xm2x_constants c = to_xm2x_constants(bits);
		std::stringstream ss;
		ss << c.C1 << ", " << c.C2 << ", " << c.C3 << ", " << c.m1;
		return ss.str();
	};

	const auto fitness = [](const bit_vector& bits, unsigned int num_threads) {
		const xm2x_constants c = to_xm2x_constants(bits);
		return sffs_fitness_test(create_xm2x_mixer(c), num_threads);
	};
	constexpr int bits = 18 + 64;
	return {bits, fitness, to_str, to_arr_str};
}

struct xm3x_constants {
	uint64_t C1;
	uint64_t C2;
	uint64_t C3;
	uint64_t C4;
	uint64_t m1;
};

inline xm3x_constants to_xm3x_constants(const bit_vector& bits) {
	using T = uint64_t;
	return {bits.get<T>(0, 6), bits.get<T>(6, 6), bits.get<T>(12, 6), bits.get<T>(18, 6), bits.get<T>(24, 64)};
}

inline mixer64 create_xm3x_mixer(const xm3x_constants& c) {
	return mixer64{
		"xm3x", [c](uint64_t x) {
			x ^= (x >> c.C1);
			x *= c.m1;
			x ^= (x >> c.C2);
			x *= c.m1;
			x ^= (x >> c.C3);
			x *= c.m1;
			x ^= (x >> c.C4);
			return x;
		}
	};
};


inline sffs_config get_xm3x_config() {
	auto to_str = [](const bit_vector& bits) {

		const xm3x_constants c = to_xm3x_constants(bits);
		std::stringstream ss;
		ss << "    x ^= x >> " << c.C1 << ";\n";
		ss << "    x *= " << c.m1 << "ull;\n";
		ss << "    x ^= x >> " << c.C2 << ";\n";
		ss << "    x *= " << c.m1 << "ull;\n";
		ss << "    x ^= x >> " << c.C3 << ";\n";
		ss << "    x *= " << c.m1 << "ull;\n";
		ss << "    x ^= x >> " << c.C4 << ";\n";
		return ss.str();
	};

	auto to_arr_str = [](const bit_vector& bits) {
		const xm3x_constants c = to_xm3x_constants(bits);
		std::stringstream ss;
		ss << c.C1 << ", " << c.C2 << ", " << c.C3 << ", " << c.C4 << ", " << c.m1;
		return ss.str();
	};

	const auto fitness = [](const bit_vector& bits, unsigned int num_threads) {
		const auto c = to_xm3x_constants(bits);
		return sffs_fitness_test(create_xm3x_mixer(c), num_threads);
	};
	constexpr int bits = 24 + 64;
	return {bits, fitness, to_str, to_arr_str};
}

}}
