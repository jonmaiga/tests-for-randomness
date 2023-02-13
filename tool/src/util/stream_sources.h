#pragma once

#include "rrc_streams.h"
#include "streams.h"
#include "util/algo.h"
#include "trng_data.h"

namespace tfr {

template <typename T>
streams<T> create_sources() {
	const auto counter1 = [] {
		return create_counter_stream<T>(1);
	};
	const auto trng = [] {
		return create_stream_from_data_by_ref<T>("trng", get_trng_data<T>());
	};

	return create_rrc_sources<T>({counter1()});
}

template <typename T>
streams<T> create_combiner_sources(combiner<T> combiner) {
	streams<T> sources;
	// 0, 0
	sources.push_back(create_combined_incremental_stream<T>(
		0, create_constant_stream<T>(0), combiner));

	// 0, b
	sources.push_back(create_combined_stream<T>(
		create_constant_stream<T>(0),
		create_counter_stream<T>(1, 0),
		combiner));

	// a, 0
	sources.push_back(create_combined_stream<T>(
		create_counter_stream<T>(1, 0),
		create_constant_stream<T>(0),
		combiner));

	const auto& mix = get_default_mixer<T>();
	streams<T> streams_a;
	streams<T> streams_b;

	// a, b
	constexpr int seed = 0;
	streams_a.push_back(create_counter_stream<T>(seed + 1, mix(1000 + seed)));
	streams_b.push_back(create_counter_stream<T>(seed + 1, mix(1 + seed)));

	// a, a
	streams_a.push_back(create_counter_stream<T>(1, mix(seed)));
	streams_b.push_back(create_counter_stream<T>(1, mix(seed)));

	// a, -a
	streams_a.push_back(create_counter_stream<T>(1, mix(seed)));
	streams_b.push_back(create_counter_stream<T>(1, -mix(seed)));

	const auto rrc_a = create_rrc_sources(streams_a);
	const auto rrc_b = create_rrc_sources(streams_b);
	for (std::size_t i = 0; i < rrc_a.size(); ++i) {
		sources.push_back(create_combined_stream(rrc_a[i], rrc_b[i], combiner));
	}

	return sources;
}

}
