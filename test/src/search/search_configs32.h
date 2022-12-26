#pragma once

#include "util/bitvector.h"
#include "util/sffs_types.h"
#include "util/sffsutils.h"

namespace mixer {namespace search32 {

struct m_constants {
	explicit m_constants(const bit_vector& bits) { m1 = bits.get(0, 32); }
	uint32_t m1;
};


inline mixer32 create_m_mixer(const bit_vector& bits) {
	return mixer32{
		"m", [c=m_constants(bits)](uint32_t x) {
			x *= c.m1;
			return x;
		}
	};
}


inline sffs_config get_m_config() {
	auto to_str = [](const bit_vector& bits) {
		const m_constants c(bits);
		std::stringstream ss;
		ss << "    x *= " << c.m1 << ";\n";
		return ss.str();
	};

	auto to_arr_str = [](const bit_vector& bits) {
		const m_constants c(bits);
		std::stringstream ss;
		ss << c.m1;
		return ss.str();
	};

	constexpr int bits = 32;
	const auto fitness = [](const bit_vector& bits, unsigned int num_threads) {
		return sffs_fitness_test(create_m_mixer(bits), num_threads);
	};
	return {bits, fitness, to_str, to_arr_str};
}

struct xmx_constants {
	uint32_t C1;
	uint32_t C2;
	uint32_t m1;
};

inline xmx_constants to_xmx_constants(const bit_vector& bits) {
	using T = uint32_t;
	return {bits.get<T>(0, 5), bits.get<T>(5, 5), bits.get<T>(10, 32)};
}

inline mixer32 create_xmx_mixer(const xmx_constants& c) {
	return mixer32{
		"xmx", [c](uint32_t x) {
			x ^= (x >> c.C1);
			x *= c.m1;
			x ^= (x >> c.C2);
			return x;
		}
	};
}

inline sffs_config get_xmx_config() {
	auto to_str = [](const bit_vector& bits) {
		const auto c = to_xmx_constants(bits);
		std::stringstream ss;
		ss << "    x ^= x >> " << c.C1 << ";\n";
		ss << "    x *= " << c.m1 << ";\n";
		ss << "    x ^= x >> " << c.C2 << ";\n";
		return ss.str();
	};

	auto to_arr_str = [](const bit_vector& bits) {
		const auto c = to_xmx_constants(bits);
		std::stringstream ss;
		ss << c.C1 << ", " << c.C2 << ", " << c.m1;
		return ss.str();
	};

	constexpr int bits = 10 + 32;
	const auto fitness = [](const bit_vector& bits, unsigned int num_threads) {
		const auto c = to_xmx_constants(bits);
		return sffs_fitness_test(create_xmx_mixer(c), num_threads);
	};
	return {bits, fitness, to_str, to_arr_str};
}

struct xm2x_constants {
	uint32_t C1;
	uint32_t C2;
	uint32_t C3;
	uint32_t m1;
	uint32_t m2;
};

inline xm2x_constants to_xm2x_constants(const bit_vector& bits) {
	using T = uint32_t;
	return {16, 15, 14, bits.get<T>(0, 32), bits.get<T>(0, 32)};
	//return {16, bits.get<T>(0, 5), bits.get<T>(5, 5), bits.get<T>(10, 32), bits.get<T>(42, 32)};
	//return {bits.get<T>(0, 5), bits.get<T>(5, 5), bits.get<T>(10, 5), bits.get<T>(15, 32)};
}


inline mixer32 create_xm2x_mixer(const xm2x_constants& c) {
	return {
		"xm2x", [c](uint32_t x) {
			x ^= (x >> c.C1);
			x *= c.m1;
			x ^= (x >> c.C2);
			x *= c.m2;
			x ^= (x >> c.C3);
			return x;
		}
	};
}

inline sffs_config get_xm2x_config() {
	auto to_str = [](const bit_vector& bits) {
		const auto c = to_xm2x_constants(bits);
		std::stringstream ss;
		ss << "    x ^= x >> " << c.C1 << ";\n";
		ss << "    x *= " << c.m1 << ";\n";
		ss << "    x ^= x >> " << c.C2 << ";\n";
		ss << "    x *= " << c.m2 << ";\n";
		ss << "    x ^= x >> " << c.C3 << ";\n";
		return ss.str();
	};

	auto to_arr_str = [](const bit_vector& bits) {
		const auto c = to_xm2x_constants(bits);
		std::stringstream ss;
		ss << c.C1 << ", " << c.C2 << ", " << c.C3 << ", " << c.m1 << ", " << c.m2;
		return ss.str();
	};

	constexpr int bits = 32;
	const auto fitness = [](const bit_vector& bits, unsigned int num_threads) {
		const auto c = to_xm2x_constants(bits);
		return sffs_fitness_test(create_xm2x_mixer(c), num_threads);
	};
	return {bits, fitness, to_str, to_arr_str};
}

struct xm3x_constants {
	uint32_t C1;
	uint32_t C2;
	uint32_t C3;
	uint32_t C4;
	uint32_t m1;
};


inline xm3x_constants to_xm3x_constants(const bit_vector& bits) {
	using T = uint32_t;
	//return {16, 15, 16, 15, bits.get<T>(0, 32)};
	//return {bits.get<T>(0, 5), bits.get<T>(5, 5), bits.get<T>(10, 5), bits.get<T>(15, 5), bits.get<T>(20, 32)};
	return {bits.get<T>(0, 5), bits.get<T>(5, 5), bits.get<T>(10, 5), bits.get<T>(15, 5), 0b01110101010101110101010101010101};
}

inline mixer32 create_xm3x_mixer(const xm3x_constants& c) {
	return mixer32{
		"xmx", [c](uint32_t x) {
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
}

inline sffs_config get_xm3x_config() {

	auto to_str = [](const bit_vector& bits) {
		const auto c = to_xm3x_constants(bits);
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
		const auto c = to_xm3x_constants(bits);
		std::stringstream ss;
		ss << c.C1 << ", " << c.C2 << ", " << c.C3 << ", " << c.C4 << ", " << c.m1;
		return ss.str();
	};

	constexpr int bits = 20; //20 + 32;
	const auto fitness = [](const bit_vector& bits, unsigned int num_threads) {
		const auto c = to_xm3x_constants(bits);
		return sffs_fitness_test(create_xm3x_mixer(c), num_threads);
	};
	return {bits, fitness, to_str, to_arr_str};
}


}}
