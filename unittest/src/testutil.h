#pragma once

#include "mixers.h"

namespace mixer {

inline stream test_stream() {
	return create_stream_from_mixer(create_counter_stream(1), mx3);
}

template <typename T>
uint64_t hash64(const T& data) {
	static_assert(sizeof(typename T::value_type) == sizeof(uint64_t));
	uint64_t h = 0;
	for (auto v : data) {
		h += mx3(*reinterpret_cast<uint64_t*>(&v));
	}
	return h;
}

}
