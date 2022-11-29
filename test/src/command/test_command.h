#pragma once

#include "eval_parallel.h"

namespace mixer {

inline void test_command() {
	using T = uint32_t;
	//const auto trng_stream = create_stream_from_data_by_ref_thread_safe<T>("trng", get_trng_data<T>());
	//const auto trng = create_mixer_from_stream<T>("trng1", trng_stream);

	const auto mixer = mix32::sffs_xmxmx_3;

	result_analyzer analyzer;
	const auto ts = test_setup<T>{
		mixer,
		create_rrc_sources<T>(),
		all_test_types
	};

	const auto rc = [&](const test_battery_result& r) {
		analyzer.add(r);
		print_battery_result(r);
		return get_meta_analysis(r).pass();
	};
	test_parallel_multi_pass(25, rc, ts);
}

}
