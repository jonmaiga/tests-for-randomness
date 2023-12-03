#pragma once

#include "rrc_streams.h"
#include "streams.h"
#include "util/algo.h"

namespace tfr {
template <typename T>
streams<T> create_sources() {
	return create_rrc_sources<T>({create_counter_stream<T>(1)});
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

	streams<T> streams_a;
	streams<T> streams_b;

	// a, b
	streams_a.push_back(create_counter_stream<T>(1, 1));
	streams_b.push_back(create_counter_stream<T>(1, 2));

	// a, a
	streams_a.push_back(create_counter_stream<T>(1, 1ull << 32));
	streams_b.push_back(create_counter_stream<T>(1, 1ull << 32));

	// a, -a
	streams_a.push_back(create_counter_stream<T>(1, 1));
	streams_b.push_back(create_counter_stream<T>(-1, 1));

	const auto rrc_a = create_rrc_sources(streams_a);
	const auto rrc_b = create_rrc_sources(streams_b);
	for (std::size_t i = 0; i < rrc_a.size(); ++i) {
		sources.push_back(create_combined_stream(rrc_a[i], rrc_b[i], combiner));
	}

	return sources;
}
}
