#pragma once

#include "rrc.h"

namespace tfr {

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
streams<T> create_rrc_sources(const streams<T>& stream_sources) {
	streams<T> rrc_sources;
	constexpr auto Bits = bit_sizeof<T>();
	for (const auto& source : stream_sources) {
		for (const auto type : rrc_types) {
			for (int rot = 0; rot < Bits; ++rot) {
				rrc_sources.push_back(add_rrc<T>(source, rot, type));
			}
		}
	}
	return rrc_sources;
}

}
