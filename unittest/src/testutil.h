#pragma once

#include "mixers32.h"
#include "mixers64.h"
#include "streams.h"
#include "trng_data.h"

namespace mixer {


template <typename T> mixer<T> get_test_mixer();

template <> inline mixer<uint64_t> get_test_mixer() { return mx3; }
template <> inline mixer<uint32_t> get_test_mixer() { return mix32::prospector; }

template <typename T = uint64_t>
stream<T> test_stream() {
	//return create_stream_from_mixer(
		//create_stream_from_data_by_ref("test_trng",
		//                               get_trng_data<T>()), get_test_mixer<T>());
	return create_stream_from_mixer<T>(create_counter_stream<T>(1), get_test_mixer<T>());
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
