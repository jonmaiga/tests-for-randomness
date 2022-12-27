#pragma once

#include "evaluate.h"
#include "mixers8.h"
#include "mixers32.h"
#include "combiners32.h"
#include "mixers64.h"
#include "source_streams.h"

namespace mixer {

inline auto create_result_callback(int max_power, bool print_intermediate_results = true) {
	return [max_power, print_intermediate_results](const test_battery_result& br) {
		const auto meta = get_meta_analysis(br);
		if (!meta) {
			return true;
		}

		const bool proceed = meta->pass() && br.power_of_two() < max_power;
		if (print_intermediate_results || !proceed) {
			print_battery_result(br);
		}
		if (!proceed) {
			std::ostringstream os;
			os << br.test_subject_name << ";" << br.power_of_two() << ";" << meta->get_failure_strength() << ";";

			std::string row = br.test_subject_name + ";" + std::to_string(br.power_of_two()) + ";";
			for (const auto& ra : get_result_analysis(br)) {
				if (!ra.analysis.pass()) {
					row += to_string(ra.key) + "(" + std::to_string(ra.analysis.get_failure_strength()) + "), ";
				}
			}
			row += "\n";
			write_append(get_config().result_path() + "result_" + std::to_string(br.bits) + ".txt", row);
		}

		return proceed;
	};
}

template <typename T>
streams<T> create_seeded_trng(int count) {
	streams<T> ts;
	const auto& data = get_trng_data<T>();
	const auto& mix = get_default_mixer<T>();
	for (int i = 1; i <= count; ++i) {
		auto indexer = create_stream_from_mixer(create_counter_stream(mix(i)), mix);
		ts.push_back(
			create_stream_from_data_by_ref<T>("trng-" + std::to_string(i), data, indexer)
		);
	}
	return ts;
}

template <typename T>
streams<T> create_seeded_counters(int count) {
	streams<T> ts;
	const auto& mix = get_default_mixer<T>();
	for (int i = 1; i <= count; ++i) {
		ts.push_back(create_counter_stream<T>(1, mix(i)));
	}
	return ts;
}

template <typename T>
test_setup<T> create_trng_test_setup() {
	return test_setup<T>{
		"trng",
		create_seeded_trng<T>(128),
		all_test_types
	};
}


template <typename T>
test_setup<T> create_combiner_test_setup(combiner<T> combiner) {

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

	return test_setup<T>{
		combiner.name,
		sources,
		all_test_types,
		mix32::xm2x
	};
}

template <typename T>
test_setup<T> create_test_setup(const mixer<T> mixer) {
	return test_setup<T>{
		mixer.name,
		create_rrc_sources<T>(),
		all_test_types,
		mixer,
	};
}

inline void test_command() {
	using T = uint32_t;
	const auto callback = create_result_callback(30, true);

	//evaluate_multi_pass(callback, create_trng_test_setup<T>());
	evaluate_multi_pass(callback, create_combiner_test_setup<T>(combine32::xmx));
	for (const auto& m : {mix32::xm2x}) {
		//evaluate_multi_pass(callback, create_test_setup(m));
	}
	write_append(get_config().result_path(), "\n");
}

}
