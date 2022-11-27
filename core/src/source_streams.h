#pragma once

#include "rrc.h"
#include "streams.h"
#include "util/algo.h"
#include "trng_data.h"


namespace mixer {

template <typename T>
stream<T> add_rrc(stream<T> source, int rotation, rrc_type type) {
	const auto name = to_string(type) + "-" + std::to_string(rotation) + "(" + source.name + ")";
	return {
		name, [source, rotation, type]() mutable -> T {
			return permute(source(), rotation, type);
		}
	};
}

template <typename T>
stream<T> create_bit_isolation_stream(stream<T> source, int bit) {
	const auto name = "bit-" + std::to_string(bit) + "(" + source.name + ")";
	return {
		name,
		[source, bit]() mutable -> T {
			return isolate_bit_by_ref(source, bit);
		}
	};
}

template <typename T>
std::vector<test_factory<T>> create_test_factories(const mixer<T>& mixer, uint64_t n) {
	const auto counter1 = [mixer, n]() {
		return test_config<T>{n, create_counter_stream<T>(1), mixer};
	};
	const auto graycode2 = [mixer, n]() {
		return test_config<T>{n, create_gray_code<T>(2), mixer};
	};
	const auto trng = [mixer, n]() {
		return test_config<T>{n, create_stream_from_data_by_ref<T>("trng", get_trng_data<T>()), mixer};
	};

	std::vector<test_factory<T>> factories{counter1}; //, trng};
	constexpr auto Bits = 8 * sizeof(T);
	for (int bit = 0; bit < Bits; ++bit) {
		const auto post_mix_permute = [bit](const stream<T>& source) {
			return create_bit_isolation_stream<T>(source, bit);
		};
		const auto bit_factory = [=]()-> test_config<T> {
			return {n, create_counter_stream<T>(1), mixer, post_mix_permute};
		};
		factories.emplace_back(bit_factory);
	}

	return factories;
}

template <typename T>
std::vector<test_factory<T>> create_rrc_test_factories(const mixer<T>& mixer, uint64_t n) {
	std::vector<test_factory<T>> factories;
	constexpr auto Bits = 8 * sizeof(T);
	for (const auto& factory : create_test_factories(mixer, n)) {
		for (const auto type : rrc_types) {
			for (int rot = 0; rot < Bits; ++rot) {
				const auto rrc_factory = [=]()-> test_config<T> {
					const auto config = factory();
					return {config.n, add_rrc<T>(config.source, rot, type), config.mix, config.stream_append_factory};
				};
				factories.emplace_back(rrc_factory);
			}
		}
	}
	return factories;
}


}
