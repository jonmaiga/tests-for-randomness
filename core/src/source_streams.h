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
std::vector<source<T>> create_sources() {
	const auto counter1 = []() {
		return source<T>{create_counter_stream<T>(1)};
	};
	const auto graycode2 = []() {
		return source<T>{create_gray_code<T>(2)};
	};
	const auto trng = []() {
		return source<T>{create_stream_from_data_by_ref<T>("trng", get_trng_data<T>())};
	};

	std::vector<source<T>> configs{counter1()}; //, trng};

	constexpr auto Bits = 8 * sizeof(T);
	for (int bit = 0; bit < Bits; ++bit) {
		const auto post_mix_permute = [bit](const stream<T>& source) {
			return create_bit_isolation_stream<T>(source, bit);
		};
		const auto bit_factory = [=]()-> source<T> {
			return {create_counter_stream<T>(1), post_mix_permute};
		};
		//configs.emplace_back(bit_factory());
	}

	return configs;
}

template <typename T>
std::vector<source<T>> create_rrc_sources() {
	std::vector<source<T>> sources;
	constexpr auto Bits = 8 * sizeof(T);
	for (const auto& config : create_sources<T>()) {
		for (const auto type : rrc_types) {
			for (int rot = 0; rot < Bits; ++rot) {
				sources.push_back({add_rrc<T>(config.stream_source, rot, type), config.stream_append_factory});
			}
		}
	}
	return sources;
}


}
