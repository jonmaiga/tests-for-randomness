#pragma once

#include "util/bit_vector.h"
#include "util/sffs_types.h"
#include "util/sffsutils.h"

namespace mixer {namespace search32 {

struct xmx_constants {
	explicit xmx_constants(const bit_vector& bits) {
		C1 = bits.get(0, 5);
		C2 = bits.get(5, 5);
		m1 = bits.get(10, 32);
	}

	uint32_t C1;
	uint32_t C2;
	uint32_t m1;
};


inline mixer32 create_xmx_mixer(const bit_vector& bits) {
	return mixer32{
		"xmx", [c=xmx_constants(bits)](uint32_t x) {
			x ^= (x >> c.C1);
			x *= c.m1;
			x ^= (x >> c.C2);
			return x;
		}
	};
}

inline sffs_config get_xmx_config() {
	auto to_str = [](const bit_vector& bits) {
		const xmx_constants c(bits);
		std::stringstream ss;
		ss << "    x ^= x >> " << c.C1 << ";\n";
		ss << "    x *= " << c.m1 << ";\n";
		ss << "    x ^= x >> " << c.C2 << ";\n";
		return ss.str();
	};

	auto to_arr_str = [](const bit_vector& bits) {
		const xmx_constants c(bits);
		std::stringstream ss;
		ss << c.C1 << ", " << c.C2 << ", " << c.m1;
		return ss.str();
	};

	constexpr int bits = 10 + 32;
	const auto fitness = [](const bit_vector& bits, unsigned int num_threads) {
		return sffs_fitness_test(create_xmx_mixer(bits), num_threads);
	};
	return {bits, fitness, to_str, to_arr_str};
}

struct xmxmx_constants {
	explicit xmxmx_constants(const bit_vector& bits) {
		C1 = bits.get(0, 5);
		C2 = bits.get(5, 5);
		C3 = bits.get(10, 5);
		m1 = bits.get(15, 32);
	}

	uint32_t C1;
	uint32_t C2;
	uint32_t C3;
	uint32_t m1;
};

inline mixer32 create_xmxmx_mixer(const bit_vector& bits) {
	return {
		"xmxmx", [c=xmxmx_constants(bits)](uint32_t x) {
			x ^= (x >> c.C1);
			x *= c.m1;
			x ^= (x >> c.C2);
			x *= c.m1;
			x ^= (x >> c.C3);
			return x;
		}
	};
}

inline sffs_config get_xmxmx_config() {
	auto to_str = [](const bit_vector& bits) {
		const xmxmx_constants c(bits);
		std::stringstream ss;
		ss << "    x ^= x >> " << c.C1 << ";\n";
		ss << "    x *= " << c.m1 << ";\n";
		ss << "    x ^= x >> " << c.C2 << ";\n";
		ss << "    x *= " << c.m1 << ";\n";
		ss << "    x ^= x >> " << c.C3 << ";\n";
		return ss.str();
	};

	auto to_arr_str = [](const bit_vector& bits) {
		const xmxmx_constants c(bits);
		std::stringstream ss;
		ss << c.C1 << ", " << c.C2 << ", " << c.C3 << ", " << c.m1;
		return ss.str();
	};

	constexpr int bits = 15 + 32;
	const auto fitness = [](const bit_vector& bits, unsigned int num_threads) {
		return sffs_fitness_test(create_xmxmx_mixer(bits), num_threads);
	};
	return {bits, fitness, to_str, to_arr_str};
}

inline sffs_config get_xmxmxmx_config() {
	struct constants {
		explicit constants(const bit_vector& bits) {
			C1 = bits.get(0, 5);
			C2 = bits.get(5, 5);
			C3 = bits.get(10, 5);
			C4 = bits.get(15, 5);
			m1 = bits.get(20, 32);
		}

		uint32_t C1;
		uint32_t C2;
		uint32_t C3;
		uint32_t C4;
		uint32_t m1;
	};

	auto factory = [](const bit_vector& bits) {
		return [c=constants(bits)](uint32_t x) {
			x ^= (x >> c.C1);
			x *= c.m1;
			x ^= (x >> c.C2);
			x *= c.m1;
			x ^= (x >> c.C3);
			x *= c.m1;
			x ^= (x >> c.C4);
			return x;
		};
	};

	auto to_str = [](const bit_vector& bits) {
		const constants c(bits);
		std::stringstream ss;
		ss << "    x ^= x >> " << c.C1 << ";\n";
		ss << "    x *= " << c.m1 << ";\n";
		ss << "    x ^= x >> " << c.C2 << ";\n";
		ss << "    x *= " << c.m1 << ";\n";
		ss << "    x ^= x >> " << c.C3 << ";\n";
		ss << "    x *= " << c.m1 << ";\n";
		ss << "    x ^= x >> " << c.C4 << ";\n";
		return ss.str();
	};

	auto to_arr_str = [](const bit_vector& bits) {
		const constants c(bits);
		std::stringstream ss;
		ss << c.C1 << ", " << c.C2 << ", " << c.C3 << ", " << c.C4 << ", " << c.m1;
		return ss.str();
	};

	constexpr int bits = 20 + 32;
	const auto fitness = [factory](const bit_vector& bits, unsigned int num_threads) {
		const auto m = mixer32{"xmxmxmx", factory(bits)};
		return sffs_fitness_test(m, num_threads);
	};
	return {bits, fitness, to_str, to_arr_str};
}


}}
