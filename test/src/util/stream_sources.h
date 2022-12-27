#pragma once

#include "rrc_streams.h"
#include "streams.h"
#include "util/algo.h"
#include "trng_data.h"

namespace mixer {

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

	return create_rrc_sources<T>({counter1()});
}

template <typename T>
streams<T> create_combiner_sources(combiner<T> combiner) {
	const auto& mix = get_default_mixer<T>();
	streams<T> sources;
	streams<T> streams_a;
	streams<T> streams_b;
	streams<T> streams_serial;
	for (int sample = 0; sample < 2; ++sample) {
		// 17 a, b
		streams_a.push_back(create_counter_stream<T>(sample + 1, mix(1000 + sample)));
		streams_b.push_back(create_counter_stream<T>(sample + 1, mix(1 + sample)));

		// 17, >23 a, a
		streams_a.push_back(create_counter_stream<T>(1, mix(sample)));
		streams_b.push_back(create_counter_stream<T>(1, mix(sample)));

		// 17 a, -a
		streams_a.push_back(create_counter_stream<T>(1, mix(sample)));
		streams_b.push_back(create_counter_stream<T>(1, -mix(sample)));

		streams_serial.push_back(create_counter_stream<T>(1, mix(sample)));
	}

	const auto rrc_a = create_rrc_sources(streams_a);
	const auto rrc_b = create_rrc_sources(streams_b);
	for (std::size_t i = 0; i < rrc_a.size(); ++i) {
		// 16, >22
		sources.push_back(create_combined_stream(rrc_a[i], rrc_b[i], combiner));
	}

	for (const auto& ss : create_rrc_sources(streams_serial)) {
		for (int draws = 2; draws <= 6; ++ draws) {
			// 16, >22
			sources.push_back({create_combined_serial_stream<T>(ss, combiner, draws)});
		}
	}
	return sources;
}

}
