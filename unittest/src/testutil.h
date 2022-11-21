#pragma once

#include "mixers64.h"
#include "streams.h"

namespace mixer {

template<typename T = uint64_t>
stream_uint64 test_stream() {
	//return create_stream_from_mixer(create_stream_from_data_by_ref("test_trng", get_trng_data()), mx3);
	return create_stream_from_mixer<T>(create_counter_stream<T>(1), mx3);
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
