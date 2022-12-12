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

struct xmxmx_constants {
	uint32_t C1;
	uint32_t C2;
	uint32_t C3;
	uint32_t m1;
};

inline xmxmx_constants to_xmxmx_constants(const bit_vector& bits) {
	using T = uint32_t;
	return {16, 15, 14, bits.get<T>(0, 32)};
	return {bits.get<T>(0, 5), bits.get<T>(5, 5), bits.get<T>(10, 5), bits.get<T>(15, 32)};
}


inline mixer32 create_xmxmx_mixer(const xmxmx_constants& c) {
	return {
		"xmxmx", [c](uint32_t x) {
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
		const auto c = to_xmxmx_constants(bits);
		std::stringstream ss;
		ss << "    x ^= x >> " << c.C1 << ";\n";
		ss << "    x *= " << c.m1 << ";\n";
		ss << "    x ^= x >> " << c.C2 << ";\n";
		ss << "    x *= " << c.m1 << ";\n";
		ss << "    x ^= x >> " << c.C3 << ";\n";
		return ss.str();
	};

	auto to_arr_str = [](const bit_vector& bits) {
		const auto c = to_xmxmx_constants(bits);
		std::stringstream ss;
		ss << c.C1 << ", " << c.C2 << ", " << c.C3 << ", " << c.m1;
		return ss.str();
	};

	constexpr int bits = 32;
	const auto fitness = [](const bit_vector& bits, unsigned int num_threads) {
		const auto c = to_xmxmx_constants(bits);
		return sffs_fitness_test(create_xmxmx_mixer(c), num_threads);
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
