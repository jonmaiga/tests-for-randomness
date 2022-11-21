#pragma once

#include "rrc.h"
#include "streams.h"
#include "util/algo.h"
#include "trng_data.h"


namespace mixer {

inline stream_uint64 add_rrc(stream_uint64 source, int rotation, rrc_type type) {
	const auto name = to_string(type) + "-" + std::to_string(rotation) + "(" + source.name + ")";
	return {
		name, [source, rotation, type]() mutable -> uint64_t {
			return permute(source(), rotation, type);
		}
	};
}

inline stream_uint64 create_bit_isolation_stream(stream_uint64 source, int bit) {
	const auto name = "bit-" + std::to_string(bit) + "(" + source.name + ")";
	return {
		name,
		[source, bit]() mutable -> uint64_t {
			return create_from_bit(source, bit);
		}
	};
}

inline std::vector<test_factory> create_test_factories(const mixer64& mixer, uint64_t n) {
	const auto counter1 = [mixer, n]() {
		return test_config{n, create_counter_stream(1), mixer};
	};
	const auto graycode2 = [mixer, n]() {
		return test_config{n, create_gray_code(2), mixer};
	};
	const auto trng = [mixer, n]() {
		return test_config{n, create_stream_from_data_by_ref("trng", get_trng_data()), mixer};
	};

	std::vector<test_factory> factories{counter1, graycode2}; //, trng};
	for (int bit = 0; bit < 64; ++bit) {
		const auto post_mix_permute = [bit](const stream_uint64& source) {
			return create_bit_isolation_stream(source, bit);
		};
		const auto bit_factory = [=]()-> test_config {
			return {n, create_counter_stream(1), mixer, post_mix_permute};
		};
		//factories.emplace_back(bit_factory);
	}

	return factories;
}

inline std::vector<test_factory> create_rrc_test_factories(const mixer64& mixer, uint64_t n) {
	std::vector<test_factory> factories;
	for (const auto& factory : create_test_factories(mixer, n)) {
		for (const auto type : rrc_types) {
			for (int rot = 0; rot < 64; ++rot) {
				const auto rrc_factory = [=]()-> test_config {
					const auto config = factory();
					return {config.n, add_rrc(config.source, rot, type), config.mix, config.stream_append_factory};
				};
				factories.emplace_back(rrc_factory);
			}
		}
	}
	return factories;
}


}
