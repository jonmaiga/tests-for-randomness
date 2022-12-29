#pragma once

#include "rrc_streams.h"
#include "streams.h"
#include "util/algo.h"
#include "trng_data.h"

namespace mixer {

template <typename T>
streams<T> create_sources() {
	const auto counter1 = [] {
		return create_counter_stream<T>(1);
	};
	const auto graycode2 = [] {
		return create_gray_code<T>(2);
	};
	const auto trng = [] {
		return create_stream_from_data_by_ref<T>("trng", get_trng_data<T>());
	};

	return create_rrc_sources<T>({counter1()});
}

template <typename T>
streams<T> create_combiner_sources(combiner<T> combiner) {
	const auto& mix = get_default_mixer<T>();
	streams<T> sources;
	// 0, 0
	sources.push_back(create_combined_incremental_stream<T>(
		0, create_constant_stream<T>(0), combiner));

	// 0, b
	sources.push_back(create_combined_stream<T>(
		create_constant_stream<T>(0),
		create_counter_stream<T>(1, 0),
		combiner));

	// a, 0 - should this be tested? 
	// sources.push_back(create_combined_stream<T>(
	// 	create_counter_stream<T>(1, 0),
	// 	create_constant_stream<T>(0),
	// 	combiner));

	streams<T> streams_a;
	streams<T> streams_b;

	// 17 a, b
	constexpr int seed = 0;
	streams_a.push_back(create_counter_stream<T>(seed + 1, mix(1000 + seed)));
	streams_b.push_back(create_counter_stream<T>(seed + 1, mix(1 + seed)));

	// 17, >23 a, a
	streams_a.push_back(create_counter_stream<T>(1, mix(seed)));
	streams_b.push_back(create_counter_stream<T>(1, mix(seed)));

	// 17 a, -a
	streams_a.push_back(create_counter_stream<T>(1, mix(seed)));
	streams_b.push_back(create_counter_stream<T>(1, -mix(seed)));

	const auto rrc_a = create_rrc_sources(streams_a);
	const auto rrc_b = create_rrc_sources(streams_b);
	for (std::size_t i = 0; i < rrc_a.size(); ++i) {
		// 16, >22
		sources.push_back(create_combined_stream(rrc_a[i], rrc_b[i], combiner));
	}

	return sources;
}

}
