#pragma once

#include <functional>
#include <string>
#include <vector>

#include "avalanche.h"
#include "streams.h"
#include "rrc.h"

namespace mixer {

struct test_result {
	std::string name;
	std::vector<avalanche_result> results;
};


struct test_config {
	stream stream;
	mixer mixer;
};

using test_factory = std::function<test_config()>;

inline stream add_rrc(const stream& source, int rotation, rrc_type type) {
	const auto name = to_string(type) + "-" + std::to_string(rotation) + "(" + source.name + ")";
	return {
		name, [source, rotation, type]()-> uint64_t {
			return permute(source(), rotation, type);
		}
	};
}

inline test_result evaluate_rrc(const test_factory& factory) {
	test_result results{"rrc"};
	for (const rrc_type type : rrc_types) {
		for (int rot = 0; rot < 64; ++rot) {
			auto cfg = factory();
			const auto rrc_stream = add_rrc(cfg.stream, rot, type);
			const auto result = avalanche_bit_independence_test(rrc_stream, cfg.mixer);
			results.results.push_back({result});
		}
	}
	return results;
}

inline test_result evaluate_rrc(const mixer& mixer, uint64_t n) {
	const auto factory = [mixer, n]() {
		return test_config{create_counter_stream(1, n), mixer};
	};
	return evaluate_rrc(factory);
}


inline test_result evaluate(const test_factory& factory) {
	const auto cfg = factory();
	test_result result{"single"};
	const auto r = avalanche_bit_independence_test(cfg.stream, cfg.mixer);
	result.results.push_back({r});
	return result;
}

inline test_result evaluate(const mixer& mixer, uint64_t n) {
	const auto factory = [mixer, n]() {
		return test_config{create_counter_stream(1, n), mixer};
	};
	return evaluate(factory);
}

inline test_result evaluate_trng(uint64_t n) {
	auto data = readBinaryMustExist<uint64_t>(R"(C:\tmp\random.org\trng_small.bin)");
	const auto factory = [data, n]() {
		const auto dummy_stream = stream{
			"dummy", [n]() mutable -> uint64_t {
				if (n-- == 0) {
					throw std::runtime_error("No more stream data.");
				}
				return 1ull;
			}
		};

		std::size_t index = 0;
		const auto trng_mixer = mixer{
			"trng",
			[index, data](uint64_t) mutable-> uint64_t {
				if (index >= data.size()) {
					throw std::runtime_error("No more mixer data.");
				}
				return data[index++];
			}
		};
		return test_config{dummy_stream, trng_mixer};
	};

	return evaluate(factory);
}


}
