#pragma once

#include "rrc.h"
#include "streams.h"


namespace mixer {

inline stream add_rrc(const stream& source, int rotation, rrc_type type) {
	const auto name = to_string(type) + "-" + std::to_string(rotation) + "(" + source.name + ")";
	return {
		name, [source, rotation, type]()-> uint64_t {
			return permute(source(), rotation, type);
		}
	};
}

inline std::vector<test_factory> create_test_factories(const mixer& mixer, uint64_t n) {
	const auto counter1 = [mixer, n]() {
		return test_config{n, create_counter_stream(1), mixer};
	};
	const auto greycode2 = [mixer, n]() {
		return test_config{n, create_gray_code(2), mixer};
	};
	const auto greycode4 = [mixer, n]() {
		return test_config{n, create_gray_code(4), mixer};
	};
	const auto greycode8 = [mixer, n]() {
		return test_config{n, create_gray_code(8), mixer};
	};
	//const auto trng = [mixer, n]() {
	//	return test_config{n, create_data_stream("trng", get_trng_data()), mixer};
	//};
	return {counter1, greycode2, greycode4, greycode8};
}

inline std::vector<test_factory> create_rrc_test_factories(const mixer& mixer, uint64_t n) {
	std::vector<test_factory> factories;
	for (const auto& factory : create_test_factories(mixer, n)) {
		for (const auto type : rrc_types) {
			for (int rot = 0; rot < 64; ++rot) {
				const auto rrc_factory = [=]()-> test_config {
					const auto config = factory();
					return {config.n, add_rrc(config.stream, rot, type), config.mixer};
				};
				factories.emplace_back(rrc_factory);
			}
		}
	}
	return factories;
}


}
