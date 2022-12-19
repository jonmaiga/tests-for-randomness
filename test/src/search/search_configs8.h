#pragma once

#include "util/bitvector.h"
#include "util/sffs_types.h"
#include "util/sffsutils.h"

namespace mixer {namespace search8 {


struct xmxmx_constants {
	uint8_t C1;
	uint8_t C2;
	uint8_t C3;
	uint8_t m1;
	uint8_t m2;
};

inline xmxmx_constants to_xmxmx_constants(const bit_vector& bits) {
	using T = uint8_t;
	return {5, 4, 3, bits.get<T>(0, 8), bits.get<T>(0, 8)};
	//return {16, bits.get<T>(0, 5), bits.get<T>(5, 5), bits.get<T>(10, 32), bits.get<T>(42, 32)};
	//return {bits.get<T>(0, 5), bits.get<T>(5, 5), bits.get<T>(10, 5), bits.get<T>(15, 32)};
}


inline mixer8 create_xmxmx_mixer(const xmxmx_constants& c) {
	return {
		"xmxmx", [c](uint8_t x) {
			x ^= (x >> c.C1);
			x *= c.m1;
			x ^= (x >> c.C2);
			x *= c.m2;
			x ^= (x >> c.C3);
			return x;
		}
	};
}

inline sffs_config get_xmxmx_config() {
	auto to_str = [](const bit_vector& bits) {
		const auto c = to_xmxmx_constants(bits);
		std::stringstream ss;
		ss << "    x ^= x >> " << c.C1 << ";\n";
		ss << "    x *= " << c.m1 << ";\n";
		ss << "    x ^= x >> " << c.C2 << ";\n";
		ss << "    x *= " << c.m2 << ";\n";
		ss << "    x ^= x >> " << c.C3 << ";\n";
		return ss.str();
	};

	auto to_arr_str = [](const bit_vector& bits) {
		const auto c = to_xmxmx_constants(bits);
		std::stringstream ss;
		ss << c.C1 << ", " << c.C2 << ", " << c.C3 << ", " << c.m1 << ", " << c.m2;
		return ss.str();
	};

	constexpr int bits = 9 + 16;
	const auto fitness = [](const bit_vector& bits, unsigned int num_threads) {
		const auto c = to_xmxmx_constants(bits);
		return sffs_fitness_test(create_xmxmx_mixer(c), num_threads);
	};
	return {bits, fitness, to_str, to_arr_str};
}

struct xm3x_constants {
	uint8_t C1;
	uint8_t C2;
	uint8_t C3;
	uint8_t C4;
	uint8_t m1;
	uint8_t m2;
	uint8_t m3;
};


inline xm3x_constants to_xm3x_constants(const bit_vector& bits) {
	using T = uint8_t;
	//return {16, 15, 16, 15, bits.get<T>(0, 32)};
	//return {bits.get<T>(0, 5), bits.get<T>(5, 5), bits.get<T>(10, 5), bits.get<T>(15, 5), bits.get<T>(20, 32)};
	return {
		bits.get<T>(0, 3),
		bits.get<T>(3, 3),
		bits.get<T>(6, 3),
		bits.get<T>(9, 3),
		bits.get<T>(12, 8),
		bits.get<T>(20, 8),
		bits.get<T>(28, 8)
	};
}

inline mixer8 create_xm3x_mixer(const xm3x_constants& c) {
	return mixer8{
		"xmx", [c](uint8_t x) {
			x ^= (x >> c.C1);
			x *= c.m1;
			x ^= (x >> c.C2);
			x *= c.m2;
			x ^= (x >> c.C3);
			x *= c.m3;
			x ^= (x >> c.C4);
			return x;
		}
	};
}

inline sffs_config get_xm3x_config() {
	auto to_str = [](const bit_vector& bits) {
		const auto c = to_xm3x_constants(bits);
		std::stringstream ss;
		ss << "    x ^= x >> " << std::to_string(c.C1) << ";\n";
		ss << "    x *= " << std::to_string(c.m1) << ";\n";
		ss << "    x ^= x >> " << std::to_string(c.C2) << ";\n";
		ss << "    x *= " << std::to_string(c.m2) << ";\n";
		ss << "    x ^= x >> " << std::to_string(c.C3) << ";\n";
		ss << "    x *= " << std::to_string(c.m3) << ";\n";
		ss << "    x ^= x >> " << std::to_string(c.C4) << ";\n";
		return ss.str();
	};

	auto to_arr_str = [](const bit_vector& bits) {
		const auto c = to_xm3x_constants(bits);
		std::stringstream ss;
		ss
			<< std::to_string(c.C1) << ", "
			<< std::to_string(c.C2) << ", "
			<< std::to_string(c.C3) << ", "
			<< std::to_string(c.C4) << ", "
			<< std::to_string(c.m1) << ", "
			<< std::to_string(c.m2) << ", "
			<< std::to_string(c.m3);
		return ss.str();
	};

	constexpr int bits = 12 + 3 * 8; //20 + 32;
	const auto fitness = [](const bit_vector& bits, unsigned int num_threads) {
		const auto c = to_xm3x_constants(bits);
		return sffs_fitness_test(create_xm3x_mixer(c), num_threads);
	};
	return {bits, fitness, to_str, to_arr_str};
}


}}
