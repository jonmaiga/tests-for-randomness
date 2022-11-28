#pragma once


namespace mixer {

inline void test_command() {
	const auto mixer = mix32::prospector;

	result_analyzer analyzer;
	for (uint64_t i = 1; i <= 10; ++i) {
		const uint64_t n = i * 10000ull;
		auto ts = test_setup{
			n, mixer,
			create_rrc_test_factories(mixer, n),
			all_test_types
		};
		std::cout << "Using " << ts.source_factories.size() << " samples per test, each with " << ts.n << " data points.\n";
		analyzer.add(test_parallel(ts));
	}
}

}
