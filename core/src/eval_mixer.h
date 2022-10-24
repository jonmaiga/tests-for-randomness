#pragma once

#include <functional>
#include <string>
#include <vector>

#include "avalanche.h"
#include "rrc.h"
#include "streams.h"
#include "tests.h"

namespace mixer {

struct test_result {
	std::string name;
	std::vector<avalanche_result> results;
};

inline stream add_rrc(const stream& source, int rotation, rrc_type type) {
	const auto name = to_string(type) + "-" + std::to_string(rotation) + "(" + source.name + ")";
	return {
		name, [source, rotation, type]()-> uint64_t {
			return permute(source(), rotation, type);
		}
	};
}

inline test_result evaluate_rrc(const std::vector<test_factory>& test_factories) {
	test_result results{"rrc"};
	for (const auto& factory : test_factories) {
		for (const rrc_type type : rrc_types) {
			for (int rot = 0; rot < 64; ++rot) {
				auto cfg = factory();
				const auto rrc_stream = add_rrc(cfg.stream, rot, type);
				const auto result = avalanche_bit_independence_test(rrc_stream, cfg.mixer);
				results.results.push_back(result);
			}
		}
	}
	return results;
}

inline test_result evaluate_rrc(const mixer& mixer, uint64_t n) {
	return evaluate_rrc(create_test_streams(mixer, n));
}

inline test_result evaluate(const std::vector<test_factory>& test_factories) {
	test_result result{"single"};
	for (const auto& factory : test_factories) {
		const auto cfg = factory();
		result.results.push_back(avalanche_bit_independence_test(cfg.stream, cfg.mixer));
	}
	return result;
}

inline test_result evaluate(const mixer& mixer, uint64_t n) {
	return evaluate(create_test_streams(mixer, n));
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

	return evaluate({factory});
}


}
