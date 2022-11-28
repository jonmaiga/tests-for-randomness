#pragma once


namespace mixer {

inline void test_command() {
	using T = uint64_t;
	const auto trng_stream = create_stream_from_data_by_ref_thread_safe<T>("trng", get_trng_data<T>());
	const auto trng = create_mixer_from_stream<T>("trng1", trng_stream);

	const auto mixer = mix64::degski64;
	for (uint64_t i = 1; i <= 20; ++i) {
		result_analyzer analyzer;
		const uint64_t n = i * 10000ull;
		auto ts = test_setup{
			n, mixer,
			create_rrc_sources<T>(),
			all_test_types
		};
		std::cout << "Using " << ts.sources.size() << " samples per test, each with " << ts.n << " data points.\n";
		analyzer.add(test_parallel(ts));
	}
}

}
