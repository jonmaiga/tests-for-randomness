#pragma once

#include "mixer_constructions.h"
#include "util/sffsutils.h"
#include "util/sffs_types.h"

namespace tfr {

template <typename T>
sffs_config get_xmx_config() {
	auto to_str = [](const bit_vector& bits) {
		const auto c = to_xmx_constants<T>(bits);
		std::stringstream ss;
		ss << "    x ^= x >> " << c.c1 << ";\n";
		ss << "    x *= " << c.m1 << "ull;\n";
		ss << "    x ^= x >> " << c.c2 << ";\n";
		return ss.str();
	};

	auto to_arr_str = [](const bit_vector& bits) {
		const auto c = to_xmx_constants<T>(bits);
		std::stringstream ss;
		ss << c.c1 << ", " << c.c2 << ", " << c.m1;
		return ss.str();
	};

	const auto fitness = [](const bit_vector& bits) {
		const auto c = to_xmx_constants<T>(bits);
		return sffs_fitness_test<T>(create_xmx_mixer<T>(c));
	};
	constexpr int bits_for_multiplier = bit_sizeof<T>();
	constexpr int bits_for_shift = shift_sizeof<T>();
	constexpr int bits = 2 * bits_for_shift + bits_for_multiplier;
	return {bits, fitness, to_str, to_arr_str};
}

template <typename T>
sffs_config get_xm2x_config() {
	auto to_str = [](const bit_vector& bits) {
		const auto c = to_xm2x_constants<T>(bits);
		std::stringstream ss;
		ss << "    x ^= x >> " << c.c1 << ";\n";
		ss << "    x *= " << c.m1 << ";\n";
		ss << "    x ^= x >> " << c.c2 << ";\n";
		ss << "    x *= " << c.m1 << ";\n";
		ss << "    x ^= x >> " << c.c3 << ";\n";
		return ss.str();
	};

	auto to_arr_str = [](const bit_vector& bits) {
		const auto c = to_xm2x_constants<T>(bits);
		std::stringstream ss;
		ss << c.c1 << ", " << c.c2 << ", " << c.c3 << ", " << c.m1;
		return ss.str();
	};

	const auto fitness = [](const bit_vector& bits) {
		const auto c = to_xm2x_constants<T>(bits);
		return sffs_fitness_test(create_xm2x_mixer(c));
	};

	constexpr int bits_for_multiplier = bit_sizeof<T>();
	constexpr int bits_for_shift = shift_sizeof<T>();
	constexpr int bits = 3 * bits_for_shift + bits_for_multiplier;
	return {bits, fitness, to_str, to_arr_str};
}


template <typename T>
sffs_config get_xm3x_config() {
	auto to_str = [](const bit_vector& bits) {
		const auto c = to_xm3x_constants<T>(bits);
		std::stringstream ss;
		ss << "    x ^= x >> " << c.c1 << ";\n";
		ss << "    x *= " << c.m1 << "ull;\n";
		ss << "    x ^= x >> " << c.c2 << ";\n";
		ss << "    x *= " << c.m1 << "ull;\n";
		ss << "    x ^= x >> " << c.c3 << ";\n";
		ss << "    x *= " << c.m1 << "ull;\n";
		ss << "    x ^= x >> " << c.c4 << ";\n";
		return ss.str();
	};

	auto to_arr_str = [](const bit_vector& bits) {
		const auto c = to_xm3x_constants<T>(bits);
		std::stringstream ss;
		ss << c.c1 << ", " << c.c2 << ", " << c.c3 << ", " << c.c4 << ", " << c.m1;
		return ss.str();
	};

	const auto fitness = [](const bit_vector& bits) {
		const auto c = to_xm3x_constants<T>(bits);
		return sffs_fitness_test(create_xm3x_mixer<T>(c));
	};

	constexpr int bits_for_multiplier = bit_sizeof<T>();
	constexpr int bits_for_shift = shift_sizeof<T>();
	constexpr int bits = 4 * bits_for_shift + bits_for_multiplier;
	return {bits, fitness, to_str, to_arr_str};
}


}
