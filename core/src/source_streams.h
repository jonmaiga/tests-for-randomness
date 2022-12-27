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
streams<T> create_sources() {
	const auto counter1 = []() {
		return create_counter_stream<T>(1);
	};
	const auto graycode2 = []() {
		return create_gray_code<T>(2);
	};
	const auto trng = []() {
		return create_stream_from_data_by_ref<T>("trng", get_trng_data<T>());
	};

	return {counter1()};
}

template <typename T>
streams<T> create_rrc_sources(const streams<T>& stream_sources) {
	streams<T> rrc_sources;
	constexpr auto Bits = 8 * sizeof(T);
	for (const auto& source : stream_sources) {
		for (const auto type : rrc_types) {
			for (int rot = 0; rot < Bits; ++rot) {
				rrc_sources.push_back(add_rrc<T>(source, rot, type));
			}
		}
	}
	return rrc_sources;
}

template <typename T>
streams<T> create_rrc_sources() {
	return create_rrc_sources(create_sources<T>());
}


}
